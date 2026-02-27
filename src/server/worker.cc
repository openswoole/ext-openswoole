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

#include <pwd.h>
#include <grp.h>
#include <sys/uio.h>
#include <sys/mman.h>

#include "openswoole_server.h"
#include "openswoole_memory.h"
#include "openswoole_msg_queue.h"
#include "openswoole_client.h"
#include "openswoole_coroutine.h"

openswoole::WorkerGlobal OpenSwooleWG = {};

namespace openswoole {
using namespace network;

static int Worker_onPipeReceive(Reactor *reactor, Event *event);
static int Worker_onStreamAccept(Reactor *reactor, Event *event);
static int Worker_onStreamRead(Reactor *reactor, Event *event);
static int Worker_onStreamPackage(Protocol *proto, Socket *sock, const char *data, uint32_t length);
static int Worker_onStreamClose(Reactor *reactor, Event *event);
static void Worker_reactor_try_to_exit(Reactor *reactor);

void Server::worker_signal_init(void) {
    /**
     * use user settings
     */
    OpenSwooleG.use_signalfd = OpenSwooleG.enable_signalfd;

    openswoole_signal_set(SIGHUP, nullptr);
    openswoole_signal_set(SIGPIPE, SIG_IGN);
    openswoole_signal_set(SIGUSR1, nullptr);
    openswoole_signal_set(SIGUSR2, nullptr);
    // swSignal_set(SIGINT, Server::worker_signal_handler);
    openswoole_signal_set(SIGTERM, Server::worker_signal_handler);
    // for test
    openswoole_signal_set(SIGVTALRM, Server::worker_signal_handler);
#ifdef SIGRTMIN
    openswoole_signal_set(SIGRTMIN, Server::worker_signal_handler);
#endif
}

void Server::worker_signal_handler(int signo) {
    if (!OpenSwooleG.running or !osw_server()) {
        return;
    }
    switch (signo) {
    case SIGTERM:
        // Event worker
        if (openswoole_event_is_available()) {
            osw_server()->stop_async_worker(OpenSwooleWG.worker);
        }
        // Task worker
        else {
            OpenSwooleWG.shutdown = true;
        }
        break;
    // for test
    case SIGVTALRM:
        openswoole_warning("SIGVTALRM coming");
        break;
    case SIGUSR1:
    case SIGUSR2:
        if (osw_logger()) {
            osw_logger()->reopen();
        }
        break;
    default:
#ifdef SIGRTMIN
        if (signo == SIGRTMIN && osw_logger()) {
            osw_logger()->reopen();
        }
#endif
        break;
    }
}

static osw_inline bool Worker_discard_data(Server *serv, Connection *conn, EventData *task) {
    if (conn == nullptr) {
        if (serv->disable_notify && !serv->discard_timeout_request) {
            return false;
        }
        goto _discard_data;
    } else {
        if (conn->closed) {
            goto _discard_data;
        } else {
            return false;
        }
    }
_discard_data:
    openswoole_error_log(OSW_LOG_WARNING,
                     OSW_ERROR_SESSION_DISCARD_TIMEOUT_DATA,
                     "[2] ignore data[%u bytes] received from session#%ld",
                     task->info.len,
                     task->info.fd);
    return true;
}

static int Worker_onStreamAccept(Reactor *reactor, Event *event) {
    Socket *sock = event->socket->accept();
    if (sock == nullptr) {
        switch (errno) {
        case EINTR:
        case EAGAIN:
            return OSW_OK;
        default:
            openswoole_sys_warning("accept() failed");
            return OSW_OK;
        }
    }

    sock->fd_type = OSW_FD_STREAM;
    sock->socket_type = OSW_SOCK_UNIX_STREAM;

    return reactor->add(sock, OSW_EVENT_READ);
}

static int Worker_onStreamRead(Reactor *reactor, Event *event) {
    Socket *conn = event->socket;
    Server *serv = (Server *) reactor->ptr;
    Protocol *protocol = &serv->stream_protocol;
    String *buffer;

    if (!event->socket->recv_buffer) {
        if (serv->buffer_pool->empty()) {
            buffer = new String(OSW_BUFFER_SIZE_STD);
        } else {
            buffer = serv->buffer_pool->front();
            serv->buffer_pool->pop();
        }
        event->socket->recv_buffer = buffer;
    } else {
        buffer = event->socket->recv_buffer;
    }

    if (protocol->recv_with_length_protocol(conn, buffer) < 0) {
        Worker_onStreamClose(reactor, event);
    }

    return OSW_OK;
}

static int Worker_onStreamClose(Reactor *reactor, Event *event) {
    Socket *sock = event->socket;
    Server *serv = (Server *) reactor->ptr;

    sock->recv_buffer->clear();
    serv->buffer_pool->push(sock->recv_buffer);
    sock->recv_buffer = nullptr;

    reactor->del(sock);
    reactor->close(reactor, sock);

    if (serv->last_stream_socket == sock) {
        serv->last_stream_socket = nullptr;
    }

    return OSW_OK;
}

static int Worker_onStreamPackage(Protocol *proto, Socket *sock, const char *data, uint32_t length) {
    Server *serv = (Server *) proto->private_data_2;

    /**
     * passing memory pointer
     */
    PacketPtr task{};
    memcpy(&task.info, data + 4, sizeof(task.info));
    task.info.flags = OSW_EVENT_DATA_PTR;
    task.data.length = length - (uint32_t) sizeof(task.info) - 4;
    task.data.str = (char *) (data + 4 + sizeof(task.info));

    /**
     * do task
     */
    serv->last_stream_socket = sock;
    serv->accept_task((EventData *) &task);
    serv->last_stream_socket = nullptr;

    /**
     * stream end
     */
    int _end = 0;
    OpenSwooleTG.reactor->write(OpenSwooleTG.reactor, sock, (void *) &_end, sizeof(_end));

    return OSW_OK;
}

typedef std::function<int(Server *, RecvData *)> TaskCallback;

static osw_inline void Worker_do_task(Server *serv, Worker *worker, EventData *task, const TaskCallback &callback) {
    RecvData recv_data;
    recv_data.info = task->info;
    recv_data.info.len = serv->get_packet(task, const_cast<char **>(&recv_data.data));

    if (callback(serv, &recv_data) == OSW_OK) {
        worker->request_count++;
        osw_atomic_fetch_add(&serv->gs->request_count, 1);
    }
}

int Server::accept_task(EventData *task) {
    Worker *worker = OpenSwooleWG.worker;
    // worker busy
    worker->status = OSW_WORKER_BUSY;

    switch (task->info.type) {
    case OSW_SERVER_EVENT_RECV_DATA: {
        Connection *conn = get_connection_verify(task->info.fd);
        if (conn) {
            if (task->info.len > 0) {
                osw_atomic_fetch_sub(&conn->recv_queued_bytes, task->info.len);
                openswoole_trace_log(OSW_TRACE_SERVER, "[Worker] len=%d, qb=%d\n", task->info.len, conn->recv_queued_bytes);
            }
            conn->last_dispatch_time = task->info.time;
        }
        if (!Worker_discard_data(this, conn, task)) {
            Worker_do_task(this, worker, task, onReceive);
        }
        break;
    }
    case OSW_SERVER_EVENT_RECV_DGRAM: {
        Worker_do_task(this, worker, task, onPacket);
        break;
    }
    case OSW_SERVER_EVENT_CLOSE: {
#ifdef OSW_USE_OPENSSL
        Connection *conn = get_connection_verify_no_ssl(task->info.fd);
        if (conn && conn->ssl_client_cert && conn->ssl_client_cert_pid == OpenSwooleG.pid) {
            delete conn->ssl_client_cert;
            conn->ssl_client_cert = nullptr;
        }
#endif
        factory->end(task->info.fd, false);
        break;
    }
    case OSW_SERVER_EVENT_CONNECT: {
#ifdef OSW_USE_OPENSSL
        // SSL client certificate
        if (task->info.len > 0) {
            Connection *conn = get_connection_verify_no_ssl(task->info.fd);
            if (conn) {
                char *cert_data = nullptr;
                size_t length = get_packet(task, &cert_data);
                conn->ssl_client_cert = new String(cert_data, length);
                conn->ssl_client_cert_pid = OpenSwooleG.pid;
            }
        }
#endif
        if (onConnect) {
            onConnect(this, &task->info);
        }
        break;
    }

    case OSW_SERVER_EVENT_BUFFER_FULL: {
        if (onBufferFull) {
            onBufferFull(this, &task->info);
        }
        break;
    }
    case OSW_SERVER_EVENT_BUFFER_EMPTY: {
        if (onBufferEmpty) {
            onBufferEmpty(this, &task->info);
        }
        break;
    }
    case OSW_SERVER_EVENT_FINISH: {
        onFinish(this, task);
        break;
    }
    case OSW_SERVER_EVENT_PIPE_MESSAGE: {
        onPipeMessage(this, task);
        break;
    }
    default:
        openswoole_warning("[Worker] error event[type=%d]", (int) task->info.type);
        break;
    }

    // worker idle
    worker->status = OSW_WORKER_IDLE;

    // maximum number of requests, process will exit.
    if (!OpenSwooleWG.run_always && worker->request_count >= OpenSwooleWG.max_request) {
        stop_async_worker(worker);
    }
    return OSW_OK;
}

void Server::worker_start_callback() {
    if (OpenSwooleG.process_id >= worker_num) {
        OpenSwooleG.process_type = OSW_PROCESS_TASKWORKER;
    } else {
        OpenSwooleG.process_type = OSW_PROCESS_WORKER;
    }

    int is_root = !geteuid();
    struct passwd *_passwd = nullptr;
    struct group *_group = nullptr;

    if (is_root) {
        // get group info
        if (!group_.empty()) {
            _group = getgrnam(group_.c_str());
            if (!_group) {
                openswoole_warning("get group [%s] info failed", group_.c_str());
            }
        }
        // get user info
        if (!user_.empty()) {
            _passwd = getpwnam(user_.c_str());
            if (!_passwd) {
                openswoole_warning("get user [%s] info failed", user_.c_str());
            }
        }
        // set process group
        if (_group && setgid(_group->gr_gid) < 0) {
            openswoole_sys_warning("setgid to [%s] failed", group_.c_str());
        }
        // set process user
        if (_passwd && setuid(_passwd->pw_uid) < 0) {
            openswoole_sys_warning("setuid to [%s] failed", user_.c_str());
        }
        // chroot
        if (!chroot_.empty()) {
            if (::chroot(chroot_.c_str()) == 0) {
                if (chdir("/") < 0) {
                    openswoole_sys_warning("chdir(\"/\") failed");
                }
            } else {
                openswoole_sys_warning("chroot(\"%s\") failed", chroot_.c_str());
            }
        }
    }

    OSW_LOOP_N(worker_num + task_worker_num) {
        Worker *worker = get_worker(i);
        if (OpenSwooleG.process_id == i) {
            continue;
        }
        if (is_worker() && worker->pipe_master) {
            worker->pipe_master->set_nonblock();
        }
    }

    if (osw_logger()->is_opened()) {
        osw_logger()->reopen();
    }

    OpenSwooleWG.worker = get_worker(OpenSwooleG.process_id);
    OpenSwooleWG.worker->status = OSW_WORKER_IDLE;

    if (is_process_mode()) {
        osw_shm_protect(session_list, PROT_READ);
        /**
         * Use only the first block of pipe_buffer memory in worker process
         */
        for (uint32_t i = 1; i < reactor_num; i++) {
            osw_free(pipe_buffers[i]);
        }
    }

#ifdef HAVE_SIGNALFD
    if (OpenSwooleG.use_signalfd && OpenSwooleTG.reactor && OpenSwooleG.signal_fd == 0) {
        openswoole_signalfd_setup(OpenSwooleTG.reactor);
    }
#endif

    call_worker_start_callback(OpenSwooleWG.worker);
}

void Server::worker_stop_callback() {
    void *hook_args[2];
    hook_args[0] = this;
    hook_args[1] = (void *) (uintptr_t) OpenSwooleG.process_id;
    if (OpenSwooleG.hooks[OSW_GLOBAL_HOOK_BEFORE_WORKER_STOP]) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_BEFORE_WORKER_STOP, hook_args);
    }
    if (onWorkerStop) {
        onWorkerStop(this, OpenSwooleG.process_id);
    }
    if (!worker_buffers.empty()) {
        openswoole_error_log(
            OSW_LOG_WARNING, OSW_ERROR_SERVER_WORKER_UNPROCESSED_DATA, "unprocessed data in the worker process buffer");
        worker_buffers.clear();
    }
}

void Server::stop_async_worker(Worker *worker) {
    worker->status = OSW_WORKER_EXIT;
    Reactor *reactor = OpenSwooleTG.reactor;

    /**
     * force to end.
     */
    if (reload_async == 0) {
        running = false;
        reactor->running = false;
        return;
    }

    // The worker process is shutting down now.
    if (reactor->wait_exit) {
        return;
    }

    // Separated from the event worker process pool
    worker = (Worker *) osw_malloc(sizeof(*worker));
    *worker = *OpenSwooleWG.worker;
    OpenSwooleWG.worker = worker;

    if (stream_socket) {
        reactor->del(stream_socket);
        stream_socket->free();
        stream_socket = nullptr;
    }

    if (worker->pipe_worker && !worker->pipe_worker->removed) {
        reactor->remove_read_event(worker->pipe_worker);
    }

    if (is_base_mode()) {
        if (is_worker()) {
            for (auto ls : ports) {
                reactor->del(ls->socket);
            }
            if (worker->pipe_master && !worker->pipe_master->removed) {
                reactor->remove_read_event(worker->pipe_master);
            }
            foreach_connection([reactor](Connection *conn) {
                if (!conn->peer_closed && !conn->socket->removed) {
                    reactor->remove_read_event(conn->socket);
                }
            });
            clear_timer();
        }
    } else {
        WorkerStopMessage msg;
        msg.pid = OpenSwooleG.pid;
        msg.worker_id = OpenSwooleG.process_id;

        // send message to manager
        if (message_box && message_box->push(&msg, sizeof(msg)) < 0) {
            running = 0;
        } else {
            openswoole_kill(gs->manager_pid, SIGIO);
        }
    }

    reactor->set_wait_exit(true);
    reactor->set_end_callback(Reactor::PRIORITY_TRY_EXIT, Worker_reactor_try_to_exit);
    OpenSwooleWG.exit_time = ::time(nullptr);

    Worker_reactor_try_to_exit(reactor);
    if (!reactor->running) {
        running = false;
    }
}

static void Worker_reactor_try_to_exit(Reactor *reactor) {
    Server *serv;
    if (OpenSwooleG.process_type == OSW_PROCESS_TASKWORKER) {
        ProcessPool *pool = (ProcessPool *) reactor->ptr;
        serv = (Server *) pool->ptr;
    } else {
        serv = (Server *) reactor->ptr;
    }
    uint8_t call_worker_exit_func = 0;

    while (1) {
        if (reactor->if_exit()) {
            reactor->running = false;
            break;
        } else {
            if (serv->onWorkerExit && call_worker_exit_func == 0) {
                serv->onWorkerExit(serv, OpenSwooleG.process_id);
                call_worker_exit_func = 1;
                continue;
            }
            int remaining_time = serv->max_wait_time - (::time(nullptr) - OpenSwooleWG.exit_time);
            if (remaining_time <= 0) {
                openswoole_error_log(
                    OSW_LOG_WARNING, OSW_ERROR_SERVER_WORKER_EXIT_TIMEOUT, "worker exit timeout, forced termination");
                reactor->running = false;
                break;
            } else {
                int timeout_msec = remaining_time * 1000;
                if (reactor->timeout_msec < 0 || reactor->timeout_msec > timeout_msec) {
                    reactor->timeout_msec = timeout_msec;
                }
            }
        }
        break;
    }
}

void Server::drain_worker_pipe() {
    for (uint32_t i = 0; i < worker_num + task_worker_num; i++) {
        Worker *worker = get_worker(i);
        if (osw_reactor()) {
            if (worker->pipe_worker) {
                osw_reactor()->drain_write_buffer(worker->pipe_worker);
            }
            if (worker->pipe_master) {
                osw_reactor()->drain_write_buffer(worker->pipe_master);
            }
        }
    }
}

/**
 * main loop [Worker]
 */
int Server::start_event_worker(Worker *worker) {
    // worker_id
    OpenSwooleG.process_id = worker->id;

    init_worker(worker);

    if (openswoole_event_init(0) < 0) {
        return OSW_ERR;
    }

    Reactor *reactor = OpenSwooleTG.reactor;
    /**
     * set pipe buffer size
     */
    for (uint32_t i = 0; i < worker_num + task_worker_num; i++) {
        Worker *_worker = get_worker(i);
        if (_worker->pipe_master) {
            _worker->pipe_master->buffer_size = UINT_MAX;
        }
        if (_worker->pipe_worker) {
            _worker->pipe_worker->buffer_size = UINT_MAX;
        }
    }

    worker->pipe_worker->set_nonblock();
    reactor->ptr = this;
    reactor->add(worker->pipe_worker, OSW_EVENT_READ);
    reactor->set_handler(OSW_FD_PIPE, Worker_onPipeReceive);

    if (dispatch_mode == DISPATCH_STREAM) {
        reactor->add(stream_socket, OSW_EVENT_READ);
        reactor->set_handler(OSW_FD_STREAM_SERVER, Worker_onStreamAccept);
        reactor->set_handler(OSW_FD_STREAM, Worker_onStreamRead);
        network::Stream::set_protocol(&stream_protocol);
        stream_protocol.private_data_2 = this;
        stream_protocol.package_max_length = UINT_MAX;
        stream_protocol.onPackage = Worker_onStreamPackage;
        buffer_pool = new std::queue<String *>;
    } else if (dispatch_mode == DISPATCH_CO_CONN_LB || dispatch_mode == DISPATCH_CO_REQ_LB) {
        reactor->set_end_callback(Reactor::PRIORITY_WORKER_CALLBACK,
                                  [worker](Reactor *) { worker->coroutine_num = Coroutine::count(); });
    }

    worker->status = OSW_WORKER_IDLE;
    worker_start_callback();

    // main loop
    reactor->wait(nullptr);
    // drain pipe buffer
    drain_worker_pipe();
    // reactor free
    openswoole_event_free();
    // worker shutdown
    worker_stop_callback();

    if (buffer_pool) {
        delete buffer_pool;
    }

    return OSW_OK;
}

/**
 * [Worker/TaskWorker/Master] Send data to ReactorThread
 */
ssize_t Server::send_to_reactor_thread(const EventData *ev_data, size_t sendn, SessionId session_id) {
    Socket *pipe_sock = get_reactor_thread_pipe(session_id, ev_data->info.reactor_id);
    if (openswoole_event_is_available()) {
        return openswoole_event_write(pipe_sock, ev_data, sendn);
    } else {
        return pipe_sock->send_blocking(ev_data, sendn);
    }
}

ssize_t Server::send_to_reactor_thread(const DataHead *head, const iovec *iov, size_t iovcnt, SessionId session_id) {
    Socket *pipe_sock = get_reactor_thread_pipe(session_id, head->reactor_id);
    if (openswoole_event_is_available()) {
        return openswoole_event_writev(pipe_sock, iov, iovcnt);
    } else {
        return pipe_sock->writev_blocking(iov, iovcnt);
    }
}

/**
 * send message from worker to another worker
 */
ssize_t Server::send_to_worker_from_worker(Worker *dst_worker, const void *buf, size_t len, int flags) {
    return dst_worker->send_pipe_message(buf, len, flags);
}

/**
 * receive data from reactor
 */
static int Worker_onPipeReceive(Reactor *reactor, Event *event) {
    ssize_t recv_n = 0;
    Server *serv = (Server *) reactor->ptr;
    PipeBuffer *pipe_buffer = serv->pipe_buffers[0];
    struct iovec buffers[2];
    int recv_chunk_count = 0;
    DataHead *info = &pipe_buffer->info;

_read_from_pipe:
    recv_n = recv(event->fd, info, sizeof(pipe_buffer->info), MSG_PEEK);
    if (recv_n < 0) {
        if (event->socket->catch_error(errno) == OSW_WAIT) {
            return OSW_OK;
        }
        return OSW_ERR;
    }

    if (pipe_buffer->info.flags & OSW_EVENT_DATA_CHUNK) {
        String *worker_buffer = serv->get_worker_buffer(info);
        if (worker_buffer == nullptr) {
            openswoole_error_log(OSW_LOG_WARNING,
                             OSW_ERROR_SERVER_WORKER_ABNORMAL_PIPE_DATA,
                             "abnormal pipeline data, msg_id=%ld, pipe_fd=%d, reactor_id=%d",
                             info->msg_id,
                             event->fd,
                             info->reactor_id);
            return OSW_OK;
        }
        size_t remain_len = pipe_buffer->info.len - worker_buffer->length;

        buffers[0].iov_base = info;
        buffers[0].iov_len = sizeof(pipe_buffer->info);
        buffers[1].iov_base = worker_buffer->str + worker_buffer->length;
        buffers[1].iov_len = OSW_MIN(serv->ipc_max_size - sizeof(pipe_buffer->info), remain_len);

        recv_n = readv(event->fd, buffers, 2);
        if (recv_n == 0) {
            openswoole_warning("receive pipeline data error, pipe_fd=%d, reactor_id=%d", event->fd, info->reactor_id);
            return OSW_ERR;
        }
        if (recv_n < 0 && event->socket->catch_error(errno) == OSW_WAIT) {
            return OSW_OK;
        }
        if (recv_n > 0) {
            worker_buffer->length += (recv_n - sizeof(pipe_buffer->info));
            openswoole_trace("append msgid=%ld, buffer=%p, n=%ld", pipe_buffer->info.msg_id, worker_buffer, recv_n);
        }

        recv_chunk_count++;

        if (!(pipe_buffer->info.flags & OSW_EVENT_DATA_END)) {
            /**
             * if the reactor thread sends too many chunks to the worker process,
             * the worker process may receive chunks all the time,
             * resulting in the worker process being unable to handle other tasks.
             * in order to make the worker process handle tasks fairly,
             * the maximum number of consecutive chunks received by the worker is limited.
             */
            if (recv_chunk_count >= OSW_WORKER_MAX_RECV_CHUNK_COUNT) {
                openswoole_trace_log(OSW_TRACE_WORKER,
                                 "worker process[%u] receives the chunk data to the maximum[%d], return to event loop",
                                 OpenSwooleG.process_id,
                                 recv_chunk_count);
                return OSW_OK;
            }
            goto _read_from_pipe;
        } else {
            /**
             * Because we don't want to split the EventData parameters into DataHead and data,
             * we store the value of the worker_buffer pointer in EventData.data.
             * The value of this pointer will be fetched in the Server_worker_get_packet function.
             */
            pipe_buffer->info.flags |= OSW_EVENT_DATA_OBJ_PTR;
            memcpy(pipe_buffer->data, &worker_buffer, sizeof(worker_buffer));
            openswoole_trace("msg_id=%ld, len=%u", pipe_buffer->info.msg_id, pipe_buffer->info.len);
        }
    } else {
        recv_n = event->socket->read(pipe_buffer, serv->ipc_max_size);
    }

    if (recv_n > 0 && serv->accept_task((EventData *) pipe_buffer) == OSW_OK) {
        if (pipe_buffer->info.flags & OSW_EVENT_DATA_END) {
            serv->worker_buffers.erase(pipe_buffer->info.msg_id);
        }
        return OSW_OK;
    }

    return OSW_ERR;
}

ssize_t Worker::send_pipe_message(const void *buf, size_t n, int flags) {
    Socket *pipe_sock;

    if (flags & OSW_PIPE_MASTER) {
        pipe_sock = pipe_master;
    } else {
        pipe_sock = pipe_worker;
    }

    // message-queue
    if (pool->use_msgqueue) {
        struct {
            long mtype;
            EventData buf;
        } msg;

        msg.mtype = id + 1;
        memcpy(&msg.buf, buf, n);

        return pool->queue->push((QueueNode *) &msg, n) ? n : -1;
    }

    if ((flags & OSW_PIPE_NONBLOCK) && OpenSwooleTG.reactor) {
        return OpenSwooleTG.reactor->write(OpenSwooleTG.reactor, pipe_sock, buf, n);
    } else {
        return pipe_sock->send_blocking(buf, n);
    }
}
}  // namespace openswoole
