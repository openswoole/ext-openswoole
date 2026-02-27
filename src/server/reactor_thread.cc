/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
 | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
 +----------------------------------------------------------------------+
 */

#include "openswoole_server.h"
#include "openswoole_memory.h"
#include "openswoole_hash.h"
#include "openswoole_client.h"
#include "openswoole_util.h"

#include <assert.h>

using std::unordered_map;

namespace openswoole {
using namespace network;

static void ReactorThread_loop(Server *serv, int reactor_id);
static int ReactorThread_init(Server *serv, Reactor *reactor, uint16_t reactor_id);
static int ReactorThread_onPipeWrite(Reactor *reactor, Event *ev);
static int ReactorThread_onPipeRead(Reactor *reactor, Event *ev);
static int ReactorThread_onRead(Reactor *reactor, Event *ev);
static int ReactorThread_onWrite(Reactor *reactor, Event *ev);
static int ReactorThread_onPacketReceived(Reactor *reactor, Event *event);
static int ReactorThread_onClose(Reactor *reactor, Event *event);
static void ReactorThread_onStreamResponse(Stream *stream, const char *data, uint32_t length);
static void ReactorThread_shutdown(Reactor *reactor);
static void ReactorThread_resume_data_receiving(Timer *timer, TimerNode *tnode);

#ifdef OSW_USE_OPENSSL
static inline enum swReturnCode ReactorThread_verify_ssl_state(Reactor *reactor, ListenPort *port, Socket *_socket) {
    Server *serv = (Server *) reactor->ptr;
    if (!_socket->ssl || _socket->ssl_state == OSW_SSL_STATE_READY) {
        return OSW_CONTINUE;
    }

    enum swReturnCode code = _socket->ssl_accept();
    if (code != OSW_READY) {
        return code;
    }

    Connection *conn = (Connection *) _socket->object;
    conn->ssl_ready = 1;
    if (!port->ssl_context->client_cert_file.empty()) {
        if (!_socket->ssl_get_peer_certificate(osw_tg_buffer())) {
            if (port->ssl_context->verify_peer) {
                return OSW_ERROR;
            }
        } else {
            if (!port->ssl_context->verify_peer || _socket->ssl_verify(port->ssl_context->allow_self_signed)) {
                SendData task;
                task.info.fd = _socket->fd;
                task.info.type = OSW_SERVER_EVENT_CONNECT;
                task.info.reactor_id = reactor->id;
                task.info.len = osw_tg_buffer()->length;
                task.data = osw_tg_buffer()->str;
                serv->factory->dispatch(&task);
                goto _delay_receive;
            } else {
                return OSW_ERROR;
            }
        }
    }

    if (serv->onConnect) {
        serv->notify((Connection *) _socket->object, OSW_SERVER_EVENT_CONNECT);
    }
_delay_receive:
    if (serv->enable_delay_receive) {
        if (reactor->del(_socket) < 0) {
            return OSW_ERROR;
        }
    }

    return OSW_READY;
}
#endif

static void ReactorThread_onStreamResponse(Stream *stream, const char *data, uint32_t length) {
    SendData response;
    Server *serv = (Server *) stream->private_data;
    Connection *conn = (Connection *) stream->private_data_2;
    SessionId session_id = stream->private_data_fd;

    if (!conn->active || session_id != conn->session_id) {
        openswoole_error_log(OSW_LOG_NOTICE, OSW_ERROR_SESSION_NOT_EXIST, "session#%ld does not exists", session_id);
        return;
    }
    if (data == nullptr) {
        Event _ev = {};
        _ev.fd = conn->fd;
        _ev.socket = conn->socket;
        osw_reactor()->trigger_close_event(&_ev);
        return;
    }

    DataHead *pkg_info = (DataHead *) data;
    response.info.fd = conn->session_id;
    response.info.type = pkg_info->type;
    response.info.len = length - sizeof(DataHead);
    response.data = data + sizeof(DataHead);
    serv->send_to_connection(&response);
}

/**
 * for udp
 */
static int ReactorThread_onPacketReceived(Reactor *reactor, Event *event) {
    int fd = event->fd;
    int ret;

    Server *serv = (Server *) reactor->ptr;
    Connection *server_sock = serv->get_connection(fd);
    network::Socket *sock = server_sock->socket;
    SendData task = {};
    DgramPacket *pkt = (DgramPacket *) osw_tg_buffer()->str;

    task.info.server_fd = fd;
    task.info.reactor_id = OpenSwooleTG.id;
    task.info.type = OSW_SERVER_EVENT_RECV_DGRAM;
    task.info.time = microtime();

    pkt->socket_addr.type = pkt->socket_type = server_sock->socket_type;

_do_recvfrom:

    ret = sock->recvfrom(pkt->data, osw_tg_buffer()->size - sizeof(*pkt), 0, &pkt->socket_addr);
    if (ret <= 0) {
        if (errno == EAGAIN) {
            return OSW_OK;
        } else {
            openswoole_sys_warning("recvfrom(%d) failed", fd);
            return OSW_ERR;
        }
    }

#ifdef OSW_SUPPORT_DTLS
    ListenPort *port = (ListenPort *) server_sock->object;

    if (port->is_dtls()) {
        dtls::Session *session = serv->accept_dtls_connection(port, &pkt->socket_addr);
        if (!session) {
            return OSW_ERR;
        }

        session->append(pkt->data, ret);

        if (!session->listen()) {
            return Server::close_connection(reactor, session->socket);
        }

        Connection *conn = (Connection *) session->socket->object;
        if (serv->single_thread) {
            if (serv->connection_incoming(reactor, conn) < 0) {
                reactor->close(reactor, session->socket);
                return OSW_OK;
            }
        } else {
            DataHead ev{};
            ev.type = OSW_SERVER_EVENT_INCOMING;
            ev.fd = conn->session_id;
            ev.reactor_id = conn->reactor_id;
            if (serv->send_to_reactor_thread((EventData *) &ev, sizeof(ev), conn->session_id) < 0) {
                reactor->close(reactor, session->socket);
                return OSW_OK;
            }
        }

        return OSW_OK;
    }
#endif

    if (pkt->socket_type == OSW_SOCK_UDP) {
        task.info.fd = *(int *) &pkt->socket_addr.addr.inet_v4.sin_addr;
    } else {
        task.info.fd = openswoole_crc32(pkt->socket_addr.get_addr(), pkt->socket_addr.len);
    }

    pkt->length = ret;
    task.info.len = sizeof(*pkt) + ret;
    task.data = (char *) pkt;

    if (!serv->factory->dispatch(&task)) {
        return OSW_ERR;
    } else {
        goto _do_recvfrom;
    }
}

/**
 * close connection
 */
int Server::close_connection(Reactor *reactor, Socket *socket) {
    Server *serv = (Server *) reactor->ptr;
    Connection *conn = (Connection *) socket->object;
    ListenPort *port = serv->get_port_by_fd(socket->fd);

    if (conn->timer) {
        openswoole_timer_del(conn->timer);
    }

    if (!socket->removed && reactor->del(socket) < 0) {
        return OSW_ERR;
    }

    osw_atomic_fetch_add(&serv->gs->close_count, 1);
    osw_atomic_fetch_sub(&serv->gs->connection_num, 1);

    openswoole_trace("Close Event.fd=%d|from=%d", socket->fd, reactor->id);

#ifdef OSW_USE_OPENSSL
    if (socket->ssl) {
        conn->socket->ssl_quiet_shutdown = conn->peer_closed;
        conn->socket->ssl_close();
    }
#ifdef OSW_SUPPORT_DTLS
    if (socket->dtls) {
        dtls::Session *session = port->dtls_sessions->find(socket->fd)->second;
        port->dtls_sessions->erase(socket->fd);
        delete session;
    }
#endif
#endif

    // free the receive memory buffer
    if (socket->recv_buffer) {
        delete socket->recv_buffer;
        socket->recv_buffer = nullptr;
    }

    osw_atomic_fetch_sub(port->connection_num, 1);

    if (port->open_http_protocol && conn->object) {
        serv->destroy_http_request(conn);
    }
    if (port->open_redis_protocol && conn->object) {
        osw_free(conn->object);
        conn->object = nullptr;
    }

#ifdef OSW_USE_SOCKET_LINGER
    if (conn->close_force || conn->close_reset) {
        struct linger linger;
        linger.l_onoff = 1;
        linger.l_linger = 0;
        if (conn->socket->set_option(SOL_SOCKET, SO_LINGER, &linger, sizeof(struct linger)) != 0) {
            openswoole_sys_warning("setsockopt(SO_LINGER) failed");
        }
    }
#endif

    Session *session = serv->get_session(conn->session_id);
    session->fd = 0;
    /**
     * reset maxfd, for connection_list
     */
    int fd = socket->fd;

    serv->lock();
    if (fd == serv->get_maxfd()) {
        int find_max_fd = fd - 1;
        openswoole_trace("set_maxfd=%d|close_fd=%d\n", find_max_fd, fd);
        // find the new max_fd
        for (; !serv->is_valid_connection(serv->get_connection(find_max_fd)) && find_max_fd > serv->get_minfd();
             find_max_fd--) {
            // pass
        }
        serv->set_maxfd(find_max_fd);
    }
    serv->unlock();

    *conn = {};
    return Reactor::_close(reactor, socket);
}

/**
 * close the connection
 */
static int ReactorThread_onClose(Reactor *reactor, Event *event) {
    Server *serv = (Server *) reactor->ptr;
    int fd = event->fd;
    DataHead notify_ev{};
    Socket *socket = event->socket;

    assert(fd % serv->reactor_num == reactor->id);
    assert(fd % serv->reactor_num == OpenSwooleTG.id);

    notify_ev.reactor_id = reactor->id;
    notify_ev.fd = fd;
    notify_ev.type = OSW_SERVER_EVENT_CLOSE;

    openswoole_trace_log(OSW_TRACE_CLOSE, "client[fd=%d] close the connection", fd);

    Connection *conn = serv->get_connection(fd);
    if (conn == nullptr || conn->active == 0) {
        return OSW_ERR;
    } else if (serv->disable_notify) {
        Server::close_connection(reactor, socket);
        return OSW_OK;
    } else if (reactor->del(socket) == 0) {
        if (conn->close_queued) {
            Server::close_connection(reactor, socket);
            return OSW_OK;
        } else {
            /**
             * peer_closed indicates that the client has closed the connection
             * and the connection is no longer available.
             */
            conn->peer_closed = 1;
            return serv->factory->notify(&notify_ev);
        }
    } else {
        return OSW_ERR;
    }
}

static void ReactorThread_shutdown(Reactor *reactor) {
    Server *serv = (Server *) reactor->ptr;
    // stop listen UDP Port
    if (serv->have_dgram_sock == 1) {
        for (auto ls : serv->ports) {
            if (ls->is_dgram()) {
                if (ls->socket->fd % serv->reactor_num != reactor->id) {
                    continue;
                }
                reactor->del(ls->socket);
            }
        }
    }

    serv->foreach_connection([serv, reactor](Connection *conn) {
        if (conn->fd % serv->reactor_num != reactor->id) {
            return;
        }
        if (!conn->peer_closed && !conn->socket->removed) {
            reactor->remove_read_event(conn->socket);
        }
    });

    reactor->set_wait_exit(true);
}

/**
 * receive data from worker process pipe
 */
static int ReactorThread_onPipeRead(Reactor *reactor, Event *ev) {
    SendData _send;

    Server *serv = (Server *) reactor->ptr;
    ReactorThread *thread = serv->get_thread(reactor->id);
    String *package = nullptr;
    PipeBuffer *resp = serv->pipe_buffers[reactor->id];

#ifdef OSW_REACTOR_RECV_AGAIN
    while (1)
#endif
    {
        ssize_t n = ev->socket->read(resp, serv->ipc_max_size);
        if (n > 0) {
            // packet chunk
            if (resp->info.flags & OSW_EVENT_DATA_CHUNK) {
                int worker_id = resp->info.server_fd;
                int key = (ev->fd << 16) + worker_id;
                auto it = thread->send_buffers.find(key);
                if (it == thread->send_buffers.end()) {
                    package = new String(OSW_BUFFER_SIZE_BIG);
                    thread->send_buffers.emplace(std::make_pair(key, package));
                } else {
                    package = it->second;
                }
                // merge data to package buffer
                package->append(resp->data, n - sizeof(resp->info));
                // wait more data
                if (!(resp->info.flags & OSW_EVENT_DATA_END)) {
                    return OSW_OK;
                }
                _send.info = resp->info;
                _send.data = package->str;
                _send.info.len = package->length;
                serv->send_to_connection(&_send);
                delete package;
                thread->send_buffers.erase(key);
            } else {
                /**
                 * connection incoming
                 */
                if (resp->info.type == OSW_SERVER_EVENT_INCOMING) {
                    Connection *conn = serv->get_connection_by_session_id(resp->info.fd);
                    if (serv->connection_incoming(reactor, conn) < 0) {
                        return reactor->close(reactor, conn->socket);
                    }
                }
                /**
                 * server shutdown
                 */
                else if (resp->info.type == OSW_SERVER_EVENT_SHUTDOWN) {
                    ReactorThread_shutdown(reactor);
                } else if (resp->info.type == OSW_SERVER_EVENT_CLOSE_FORCE) {
                    SessionId session_id = resp->info.fd;
                    Connection *conn = serv->get_connection_verify_no_ssl(session_id);

                    if (!conn) {
                        openswoole_error_log(OSW_LOG_NOTICE,
                                         OSW_ERROR_SESSION_NOT_EXIST,
                                         "force close connection failed, session#%ld does not exist",
                                         session_id);
                        return OSW_OK;
                    }

                    if (serv->disable_notify || conn->close_force) {
                        return Server::close_connection(reactor, conn->socket);
                    }

#ifdef OSW_USE_OPENSSL
                    /**
                     * SSL connections that have not completed the handshake,
                     * do not need to notify the workers, just close
                     */
                    if (conn->ssl && !conn->ssl_ready) {
                        return Server::close_connection(reactor, conn->socket);
                    }
#endif

                    conn->close_force = 1;
                    Event _ev = {};
                    _ev.fd = conn->fd;
                    _ev.socket = conn->socket;
                    reactor->trigger_close_event(&_ev);
                } else {
                    _send.info = resp->info;
                    _send.data = resp->data;
                    serv->send_to_connection(&_send);
                }
            }
        } else if (errno == EAGAIN) {
            return OSW_OK;
        } else {
            openswoole_sys_warning("read(worker_pipe) failed");
            return OSW_ERR;
        }
    }

    return OSW_OK;
}

ssize_t Server::send_to_worker_from_master(Worker *worker, const iovec *iov, size_t iovcnt) {
    if (OpenSwooleTG.reactor) {
        ReactorThread *thread = get_thread(OpenSwooleTG.id);
        Socket *socket = &thread->pipe_sockets[worker->pipe_master->fd];
        return openswoole_event_writev(socket, iov, iovcnt);
    } else {
        return worker->pipe_master->writev_blocking(iov, iovcnt);
    }
}

/**
 * [ReactorThread] worker pipe can write.
 */
static int ReactorThread_onPipeWrite(Reactor *reactor, Event *ev) {
    int ret;

    Server *serv = (Server *) reactor->ptr;
    Buffer *buffer = ev->socket->out_buffer;

    while (!Buffer::empty(buffer)) {
        BufferChunk *chunk = buffer->front();
        EventData *send_data = (EventData *) chunk->value.ptr;

        // server actively closed connection, should discard the data
        if (Server::is_stream_event(send_data->info.type)) {
            // send_data->info.fd is session_id
            Connection *conn = serv->get_connection_verify(send_data->info.fd);
            if (conn) {
                conn->last_send_time = microtime();
                if (conn->closed) {
                    openswoole_error_log(OSW_LOG_NOTICE,
                                     OSW_ERROR_SESSION_CLOSED_BY_SERVER,
                                     "Session#%ld is closed by server",
                                     send_data->info.fd);
                _discard:
                    buffer->pop();
                    continue;
                }
            } else if (serv->discard_timeout_request) {
                openswoole_error_log(OSW_LOG_WARNING,
                                 OSW_ERROR_SESSION_DISCARD_TIMEOUT_DATA,
                                 "[1] ignore data[%u bytes] received from session#%ld",
                                 send_data->info.len,
                                 send_data->info.fd);
                goto _discard;
            }
        }

        ret = ev->socket->send(chunk->value.ptr, chunk->length, 0);
        if (ret < 0) {
            return (ev->socket->catch_error(errno) == OSW_WAIT) ? OSW_OK : OSW_ERR;
        } else {
            buffer->pop();
        }
    }

    if (Buffer::empty(buffer)) {
        if (reactor->remove_write_event(ev->socket) < 0) {
            openswoole_sys_warning("reactor->set(%d) failed", ev->fd);
        }
    }

    return OSW_OK;
}

void Server::init_reactor(Reactor *reactor) {
    // support 64K packet
    if (have_dgram_sock) {
        osw_tg_buffer()->extend();
    }
    // UDP Packet
    reactor->set_handler(OSW_FD_DGRAM_SERVER, ReactorThread_onPacketReceived);
    // Write
    reactor->set_handler(OSW_FD_SESSION | OSW_EVENT_WRITE, ReactorThread_onWrite);
    // Read
    reactor->set_handler(OSW_FD_SESSION | OSW_EVENT_READ, ReactorThread_onRead);

    if (dispatch_mode == DISPATCH_STREAM) {
        Client::init_reactor(reactor);
    }

    // listen the all tcp port
    for (auto port : ports) {
        if (port->is_dgram()
#ifdef OSW_SUPPORT_DTLS
            && !(port->is_dtls())
#endif
        ) {
            continue;
        }
        init_port_protocol(port);
    }
}

static int ReactorThread_onRead(Reactor *reactor, Event *event) {
    Server *serv = (Server *) reactor->ptr;
    Connection *conn = serv->get_connection(event->fd);
    /**
     * invalid event
     * The server has been actively closed the connection, the client also initiated off, fd has been reused.
     */
    if (!conn || conn->server_fd == 0) {
        return OSW_OK;
    }
    ListenPort *port = serv->get_port_by_fd(event->fd);
#ifdef OSW_USE_OPENSSL
#ifdef OSW_SUPPORT_DTLS
    if (port->is_dtls()) {
        dtls::Buffer *buffer = (dtls::Buffer *) osw_malloc(sizeof(*buffer) + OSW_BUFFER_SIZE_UDP);
        buffer->length = event->socket->read(buffer->data, OSW_BUFFER_SIZE_UDP);
        dtls::Session *session = port->dtls_sessions->find(event->fd)->second;
        session->append(buffer);
        if (!session->listened && !session->listen()) {
            Server::close_connection(reactor, event->socket);
            return OSW_OK;
        }
    }
#endif
    enum swReturnCode code = ReactorThread_verify_ssl_state(reactor, port, event->socket);
    switch (code) {
    case OSW_ERROR:
        return Server::close_connection(reactor, event->socket);
    case OSW_READY:
#ifdef OSW_SUPPORT_DTLS
        if (event->socket->dtls) {
            return OSW_OK;
        }
#endif
        break;
    case OSW_WAIT:
        return OSW_OK;
    case OSW_CONTINUE:
        break;
    default:
        abort();
    }
#endif

    conn->last_recv_time = microtime();

    int retval = port->onRead(reactor, port, event);
    if (!conn->active) {
        return retval;
    }
    if (serv->is_process_mode() && serv->max_queued_bytes && conn->recv_queued_bytes > serv->max_queued_bytes) {
        conn->waiting_time = 1;
        conn->timer = openswoole_timer_add(conn->waiting_time, false, ReactorThread_resume_data_receiving, event->socket);
        if (conn->timer) {
            reactor->remove_read_event(event->socket);
        }
    }
    return retval;
}

static int ReactorThread_onWrite(Reactor *reactor, Event *ev) {
    int ret;
    Server *serv = (Server *) reactor->ptr;
    Socket *socket = ev->socket;
    int fd = ev->fd;

    if (serv->is_process_mode()) {
        assert(fd % serv->reactor_num == reactor->id);
        assert(fd % serv->reactor_num == OpenSwooleTG.id);
    }

    Connection *conn = serv->get_connection(fd);
    if (conn == nullptr || conn->active == 0) {
        return OSW_ERR;
    }

    openswoole_trace_log(OSW_TRACE_REACTOR,
                     "fd=%d, conn->close_notify=%d, serv->disable_notify=%d, conn->close_force=%d",
                     fd,
                     conn->close_notify,
                     serv->disable_notify,
                     conn->close_force);

    if (conn->close_notify) {
#ifdef OSW_USE_OPENSSL
        if (socket->ssl && socket->ssl_state != OSW_SSL_STATE_READY) {
            return Server::close_connection(reactor, socket);
        }
#endif
        serv->notify(conn, OSW_SERVER_EVENT_CLOSE);
        conn->close_notify = 0;
        return OSW_OK;
    } else if (serv->disable_notify && conn->close_force) {
        return Server::close_connection(reactor, socket);
    }

    while (!Buffer::empty(socket->out_buffer)) {
        BufferChunk *chunk = socket->out_buffer->front();
        if (chunk->type == BufferChunk::TYPE_CLOSE) {
            return reactor->close(reactor, socket);
        } else if (chunk->type == BufferChunk::TYPE_SENDFILE) {
            ret = socket->handle_sendfile();
        } else {
            ret = socket->handle_send();
            if (OSW_OK == ret) {
                conn->send_queued_bytes = socket->out_buffer->length();
            }
        }

        if (ret < 0) {
            if (socket->close_wait) {
                conn->close_errno = errno;
                return reactor->trigger_close_event(ev);
            } else if (socket->send_wait) {
                break;
            }
        }
    }

    if (conn->overflow && socket->out_buffer->length() < socket->buffer_size) {
        conn->overflow = 0;
    }

    if (serv->onBufferEmpty && conn->high_watermark) {
        ListenPort *port = serv->get_port_by_fd(fd);
        if (socket->out_buffer->length() <= port->buffer_low_watermark) {
            conn->high_watermark = 0;
            serv->notify(conn, OSW_SERVER_EVENT_BUFFER_EMPTY);
        }
    }

    if (socket->send_timer) {
        openswoole_timer_del(socket->send_timer);
        socket->send_timer = nullptr;
    }

    // remove EPOLLOUT event
    if (!conn->peer_closed && !socket->removed && Buffer::empty(socket->out_buffer)) {
        reactor->set(socket, OSW_EVENT_READ);
    }
    return OSW_OK;
}

int Server::create_reactor_threads() {
    /**
     * init reactor thread pool
     */
    reactor_threads = new ReactorThread[reactor_num]();
    /**
     * alloc the memory for connection_list
     */
    connection_list = (Connection *) osw_shm_calloc(max_connection, sizeof(Connection));
    if (connection_list == nullptr) {
        openswoole_error("calloc[1] failed");
        return OSW_ERR;
    }
    reactor_pipe_num = worker_num / reactor_num;
    return OSW_OK;
}

/**
 * [master]
 */
int Server::start_reactor_threads() {
    if (openswoole_event_init(0) < 0) {
        return OSW_ERR;
    }

    Reactor *reactor = OpenSwooleTG.reactor;

#ifdef HAVE_SIGNALFD
    if (OpenSwooleG.use_signalfd) {
        openswoole_signalfd_setup(reactor);
    }
#endif

    for (auto iter = ports.begin(); iter != ports.end(); iter++) {
        auto port = *iter;
        if (port->is_dgram()) {
            continue;
        }
        if (port->listen() < 0) {
            openswoole_event_free();
            return OSW_ERR;
        }
        reactor->add(port->socket, OSW_EVENT_READ);
    }

    /**
     * create reactor thread
     */
    ReactorThread *thread;

    store_listen_socket();

    if (single_thread) {
        ReactorThread_init(this, reactor, 0);
        goto _init_master_thread;
    }
    /**
     * multi-threads
     */
    else {
        /**
         * set a special id
         */
        reactor->id = reactor_num;
        OpenSwooleTG.id = reactor_num;
    }

#ifdef HAVE_PTHREAD_BARRIER
    // init thread barrier
    pthread_barrier_init(&barrier, nullptr, reactor_num + 1);
#endif
    OSW_LOOP_N(reactor_num) {
        thread = &(reactor_threads[i]);
        thread->thread = std::thread(ReactorThread_loop, this, i);
    }
#ifdef HAVE_PTHREAD_BARRIER
    // wait reactor thread
    pthread_barrier_wait(&barrier);
#else
    OSW_START_SLEEP;
#endif

_init_master_thread:

    /**
     * heartbeat thread
     */
    if (heartbeat_check_interval >= 1) {
        start_heartbeat_thread();
    }

    OpenSwooleTG.type = THREAD_MASTER;
    OpenSwooleTG.update_time = 1;
    OpenSwooleTG.reactor = reactor;

    if (OpenSwooleTG.timer && OpenSwooleTG.timer->get_reactor() == nullptr) {
        OpenSwooleTG.timer->reinit(reactor);
    }

    OpenSwooleG.pid = getpid();
    OpenSwooleG.process_type = OSW_PROCESS_MASTER;

    reactor->ptr = this;
    reactor->set_handler(OSW_FD_STREAM_SERVER, Server::accept_connection);

    if (hooks[Server::HOOK_MASTER_START]) {
        call_hook(Server::HOOK_MASTER_START, this);
    }

    /**
     * 1 second timer
     */
    if ((master_timer = openswoole_timer_add(1000, true, Server::timer_callback, this)) == nullptr) {
        openswoole_event_free();
        return OSW_ERR;
    }

    if (onStart) {
        onStart(this);
    }

    return openswoole_event_wait();
}

static int ReactorThread_init(Server *serv, Reactor *reactor, uint16_t reactor_id) {
    ReactorThread *thread = serv->get_thread(reactor_id);

    reactor->ptr = serv;
    reactor->id = reactor_id;
    reactor->wait_exit = 0;
    reactor->max_socket = serv->get_max_connection();
    reactor->close = Server::close_connection;

    reactor->set_exit_condition(Reactor::EXIT_CONDITION_DEFAULT, [thread](Reactor *reactor, size_t &event_num) -> bool {
        return event_num == (size_t) thread->pipe_num;
    });

    reactor->default_error_handler = ReactorThread_onClose;

    reactor->set_handler(OSW_FD_PIPE | OSW_EVENT_READ, ReactorThread_onPipeRead);
    reactor->set_handler(OSW_FD_PIPE | OSW_EVENT_WRITE, ReactorThread_onPipeWrite);

    // listen UDP port
    if (serv->have_dgram_sock == 1) {
        for (auto ls : serv->ports) {
            if (ls->is_stream()) {
                continue;
            }
            int server_fd = ls->socket->fd;
            if (server_fd % serv->reactor_num != reactor_id) {
                continue;
            }
            Connection *serv_sock = serv->get_connection(server_fd);
            if (ls->type == OSW_SOCK_UDP) {
                serv_sock->info.addr.inet_v4.sin_port = htons(ls->port);
            } else if (ls->type == OSW_SOCK_UDP6) {
                serv_sock->info.addr.inet_v6.sin6_port = htons(ls->port);
            }
            serv_sock->fd = server_fd;
            serv_sock->socket_type = ls->type;
            serv_sock->object = ls;
            ls->thread_id = pthread_self();
            if (reactor->add(ls->socket, OSW_EVENT_READ) < 0) {
                return OSW_ERR;
            }
        }
    }

    serv->init_reactor(reactor);

    int max_pipe_fd = serv->get_worker(serv->worker_num - 1)->pipe_master->fd + 2;
    thread->pipe_sockets = (Socket *) osw_calloc(max_pipe_fd, sizeof(Socket));
    if (!thread->pipe_sockets) {
        openswoole_sys_error("calloc(%d, %ld) failed", max_pipe_fd, sizeof(Socket));
        return OSW_ERR;
    }

    for (uint32_t i = 0; i < serv->worker_num; i++) {
        int pipe_fd = serv->workers[i].pipe_master->fd;
        Socket *socket = &thread->pipe_sockets[pipe_fd];

        socket->fd = pipe_fd;
        socket->fd_type = OSW_FD_PIPE;
        socket->buffer_size = UINT_MAX;

        if (i % serv->reactor_num != reactor_id) {
            continue;
        }

        socket->set_nonblock();

        if (reactor->add(socket, OSW_EVENT_READ) < 0) {
            return OSW_ERR;
        }
        if (thread->notify_pipe == nullptr) {
            thread->notify_pipe = serv->workers[i].pipe_worker;
        }
        thread->pipe_num++;
    }

    return OSW_OK;
}

/**
 * ReactorThread main Loop
 */
static void ReactorThread_loop(Server *serv, int reactor_id) {
    OpenSwooleTG.id = reactor_id;
    OpenSwooleTG.type = Server::THREAD_REACTOR;

    OpenSwooleTG.buffer_stack = new String(OSW_STACK_BUFFER_SIZE);
    ON_SCOPE_EXIT {
        delete OpenSwooleTG.buffer_stack;
        OpenSwooleTG.buffer_stack = nullptr;
    };

    if (openswoole_event_init(0) < 0) {
        return;
    }

    ReactorThread *thread = serv->get_thread(reactor_id);
    Reactor *reactor = osw_reactor();

#ifdef HAVE_CPU_AFFINITY
    // cpu affinity setting
    if (serv->open_cpu_affinity) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);

        if (serv->cpu_affinity_available_num) {
            CPU_SET(serv->cpu_affinity_available[reactor_id % serv->cpu_affinity_available_num], &cpu_set);
        } else {
            CPU_SET(reactor_id % OSW_CPU_NUM, &cpu_set);
        }

        if (0 != pthread_setaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set)) {
            openswoole_sys_warning("pthread_setaffinity_np() failed");
        }
    }
#endif

    openswoole_signal_block_all();

    if (ReactorThread_init(serv, reactor, reactor_id) < 0) {
        return;
    }

    // wait other thread
#ifdef HAVE_PTHREAD_BARRIER
    pthread_barrier_wait(&serv->barrier);
#else
    OSW_START_SLEEP;
#endif
    // main loop
    openswoole_event_wait();

    for (auto it = thread->send_buffers.begin(); it != thread->send_buffers.end(); it++) {
        delete it->second;
    }
    osw_free(thread->pipe_sockets);
}

static void ReactorThread_resume_data_receiving(Timer *timer, TimerNode *tnode) {
    Socket *_socket = (Socket *) tnode->data;
    Connection *conn = (Connection *) _socket->object;

    if (conn->recv_queued_bytes > osw_server()->max_queued_bytes) {
        if (conn->waiting_time != 1024) {
            conn->waiting_time *= 2;
        }
        conn->timer = openswoole_timer_add(conn->waiting_time, false, ReactorThread_resume_data_receiving, _socket);
        if (conn->timer) {
            return;
        }
    }

    timer->get_reactor()->add_read_event(_socket);
    conn->timer = nullptr;
}

/**
 * dispatch request data [only data frame]
 */
int Server::dispatch_task(Protocol *proto, Socket *_socket, const char *data, uint32_t length) {
    Server *serv = (Server *) proto->private_data_2;
    SendData task;

    Connection *conn = (Connection *) _socket->object;

    osw_memset_zero(&task.info, sizeof(task.info));
    task.info.server_fd = conn->server_fd;
    task.info.reactor_id = conn->reactor_id;
    task.info.ext_flags = proto->ext_flags;
    proto->ext_flags = 0;
    task.info.type = OSW_SERVER_EVENT_RECV_DATA;
    task.info.time = conn->last_recv_time;

    openswoole_trace("send string package, size=%ld bytes", (long) length);

    if (serv->stream_socket_file) {
        Stream *stream = Stream::create(serv->stream_socket_file, 0, OSW_SOCK_UNIX_STREAM);
        if (!stream) {
            return OSW_ERR;
        }
        stream->response = ReactorThread_onStreamResponse;
        stream->private_data = serv;
        stream->private_data_2 = conn;
        stream->private_data_fd = conn->session_id;
        ListenPort *port = serv->get_port_by_fd(conn->fd);
        stream->set_max_length(port->protocol.package_max_length);

        task.info.fd = conn->session_id;

        if (stream->send((char *) &task.info, sizeof(task.info)) < 0) {
        _cancel:
            stream->cancel = 1;
            delete stream;
            return OSW_ERR;
        }
        if (stream->send(data, length) < 0) {
            goto _cancel;
        }
        return OSW_OK;
    } else {
        task.info.fd = conn->fd;
        task.info.len = length;
        task.data = data;
        if (!serv->factory->dispatch(&task)) {
            return OSW_ERR;
        }
        if (length > 0) {
            osw_atomic_fetch_add(&conn->recv_queued_bytes, length);
            openswoole_trace_log(OSW_TRACE_SERVER, "[Master] len=%d, qb=%d\n", length, conn->recv_queued_bytes);
        }
        return OSW_OK;
    }
}

void Server::join_reactor_thread() {
    if (single_thread) {
        return;
    }
    ReactorThread *thread;
    /**
     * Shutdown heartbeat thread
     */
    if (heartbeat_thread.joinable()) {
        openswoole_trace_log(OSW_TRACE_SERVER, "terminate heartbeat thread");
        if (pthread_cancel(heartbeat_thread.native_handle()) < 0) {
            openswoole_sys_warning("pthread_cancel(%ld) failed", (ulong_t) heartbeat_thread.native_handle());
        }
        // wait thread
        heartbeat_thread.join();
    }
    /**
     * kill threads
     */
    for (int i = 0; i < reactor_num; i++) {
        thread = get_thread(i);
        if (thread->notify_pipe) {
            DataHead ev = {};
            ev.type = OSW_SERVER_EVENT_SHUTDOWN;
            if (thread->notify_pipe->send_blocking((void *) &ev, sizeof(ev)) < 0) {
                goto _cancel;
            }
        } else {
        _cancel:
            if (pthread_cancel(thread->thread.native_handle()) < 0) {
                openswoole_sys_warning("pthread_cancel(%ld) failed", (long) thread->thread.native_handle());
            }
        }
        thread->thread.join();
    }
}

void Server::destroy_reactor_threads() {
    osw_shm_free(connection_list);
    delete[] reactor_threads;

    if (message_box) {
        message_box->destroy();
    }
}

void Server::start_heartbeat_thread() {
    heartbeat_thread = std::thread([this]() {
        openswoole_signal_block_all();

        OpenSwooleTG.type = THREAD_HEARTBEAT;
        OpenSwooleTG.id = reactor_num;

        while (running) {
            double now = microtime();
            foreach_connection([this, now](Connection *conn) {
                SessionId session_id = conn->session_id;
                if (session_id <= 0) {
                    return;
                }
                if (is_healthy_connection(now, conn)) {
                    return;
                }
                DataHead ev{};
                ev.type = OSW_SERVER_EVENT_CLOSE_FORCE;
                // convert fd to session_id, in order to verify the connection before the force close connection
                ev.fd = session_id;
                Socket *_pipe_sock = get_reactor_thread_pipe(session_id, conn->reactor_id);
                _pipe_sock->send_blocking((void *) &ev, sizeof(ev));
            });
            sleep(heartbeat_check_interval);
        }
    });
}

}  // namespace openswoole
