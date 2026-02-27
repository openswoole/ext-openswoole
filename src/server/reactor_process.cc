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

namespace openswoole {
using network::Socket;

static int ReactorProcess_loop(ProcessPool *pool, Worker *worker);
static int ReactorProcess_onPipeRead(Reactor *reactor, Event *event);
static int ReactorProcess_onClose(Reactor *reactor, Event *event);
static void ReactorProcess_onTimeout(Timer *timer, TimerNode *tnode);

#ifdef HAVE_REUSEPORT
static int ReactorProcess_reuse_port(ListenPort *ls);
#endif

static bool Server_is_single(Server *serv) {
    return serv->worker_num == 1 && serv->task_worker_num == 0 && serv->max_request == 0 &&
           serv->user_worker_list.empty();
}

int Server::create_reactor_processes() {
    reactor_num = worker_num;
    connection_list = (Connection *) osw_calloc(max_connection, sizeof(Connection));
    if (connection_list == nullptr) {
        openswoole_sys_warning("calloc[2](%d) failed", (int) (max_connection * sizeof(Connection)));
        return OSW_ERR;
    }
    return OSW_OK;
}

void Server::destroy_reactor_processes() {
    osw_free(connection_list);
}

int Server::start_reactor_processes() {
    single_thread = 1;

    // listen TCP
    if (have_stream_sock == 1) {
        for (auto ls : ports) {
            if (ls->is_dgram()) {
                continue;
            }
#ifdef HAVE_REUSEPORT
            if (enable_reuse_port) {
                if (::close(ls->socket->fd) < 0) {
                    openswoole_sys_warning("close(%d) failed", ls->socket->fd);
                }
                delete ls->socket;
                ls->socket = nullptr;
                continue;
            } else
#endif
            {
                // listen server socket
                if (ls->listen() < 0) {
                    return OSW_ERR;
                }
            }
        }
    }

    ProcessPool *pool = &gs->event_workers;
    *pool = {};
    if (pool->create(worker_num, 0, OSW_IPC_UNIXSOCK) < 0) {
        return OSW_ERR;
    }
    pool->set_max_request(max_request, max_request_grace);

    /**
     * store to ProcessPool object
     */
    gs->event_workers.ptr = this;
    gs->event_workers.max_wait_time = max_wait_time;
    gs->event_workers.use_msgqueue = 0;
    gs->event_workers.main_loop = ReactorProcess_loop;
    gs->event_workers.onWorkerNotFound = Server::wait_other_worker;

    OSW_LOOP_N(worker_num) {
        gs->event_workers.workers[i].pool = &gs->event_workers;
        gs->event_workers.workers[i].id = i;
        gs->event_workers.workers[i].type = OSW_PROCESS_WORKER;
    }

    // single worker
    if (Server_is_single(this)) {
        int retval = ReactorProcess_loop(&gs->event_workers, &gs->event_workers.workers[0]);
        if (retval == OSW_OK) {
            gs->event_workers.destroy();
        }
        return retval;
    }

    OSW_LOOP_N(worker_num) {
        create_worker(&gs->event_workers.workers[i]);
    }

    // task workers
    if (task_worker_num > 0) {
        if (create_task_workers() < 0) {
            return OSW_ERR;
        }
        if (gs->task_workers.start() < 0) {
            return OSW_ERR;
        }
    }

    // create user worker process
    if (!user_worker_list.empty()) {
        user_workers = (Worker *) osw_shm_calloc(get_user_worker_num(), sizeof(Worker));
        if (user_workers == nullptr) {
            openswoole_sys_warning("gmalloc[server->user_workers] failed");
            return OSW_ERR;
        }
        for (auto worker : user_worker_list) {
            /**
             * store the pipe object
             */
            if (worker->pipe_object) {
                store_pipe_fd(worker->pipe_object);
            }
            spawn_user_worker(worker);
        }
    }

    /**
     * manager process is the same as the master process
     */
    OpenSwooleG.pid = gs->manager_pid = getpid();
    OpenSwooleG.process_type = OSW_PROCESS_MANAGER;

    /**
     * manager process can not use signalfd
     */
    OpenSwooleG.use_signalfd = 0;

    gs->event_workers.start();

    init_signal_handler();

    if (onStart) {
        openswoole_warning("The onStart event with OPENSWOOLE_BASE is deprecated");
        onStart(this);
    }

    if (onManagerStart) {
        onManagerStart(this);
    }

    gs->event_workers.wait();
    gs->event_workers.shutdown();

    kill_user_workers();

    if (onManagerStop) {
        onManagerStop(this);
    }

    OSW_LOOP_N(worker_num) {
        destroy_worker(&gs->event_workers.workers[i]);
    }

    return OSW_OK;
}

static int ReactorProcess_onPipeRead(Reactor *reactor, Event *event) {
    EventData task;
    SendData _send;
    Server *serv = (Server *) reactor->ptr;
    Factory *factory = serv->factory;
    String *output_buffer;

    ssize_t retval = read(event->fd, &task, sizeof(task));
    if (retval <= 0) {
        return OSW_ERR;
    } else if ((size_t) retval != task.info.len + sizeof(_send.info)) {
        openswoole_warning("bad pipeline data");
        return OSW_OK;
    }

    switch (task.info.type) {
    case OSW_SERVER_EVENT_PIPE_MESSAGE:
        serv->onPipeMessage(serv, &task);
        break;
    case OSW_SERVER_EVENT_FINISH:
        serv->onFinish(serv, &task);
        break;
    case OSW_SERVER_EVENT_SEND_FILE:
        _send.info = task.info;
        _send.data = task.data;
        factory->finish(&_send);
        break;
    case OSW_SERVER_EVENT_PROXY_START:
    case OSW_SERVER_EVENT_PROXY_END:
        if (task.info.reactor_id < 0 || task.info.reactor_id >= (int16_t) serv->get_all_worker_num()) {
            openswoole_warning("invalid worker_id=%d", task.info.reactor_id);
            return OSW_OK;
        }
        output_buffer = OpenSwooleWG.output_buffer[task.info.reactor_id];
        output_buffer->append(task.data, task.info.len);
        if (task.info.type == OSW_SERVER_EVENT_PROXY_END) {
            memcpy(&_send.info, &task.info, sizeof(_send.info));
            _send.info.type = OSW_SERVER_EVENT_RECV_DATA;
            _send.data = output_buffer->str;
            _send.info.len = output_buffer->length;
            factory->finish(&_send);
            output_buffer->clear();
        }
        break;
    case OSW_SERVER_EVENT_CLOSE:
        factory->end(task.info.fd, Server::CLOSE_ACTIVELY);
        break;
    default:
        break;
    }
    return OSW_OK;
}

static int ReactorProcess_alloc_output_buffer(size_t n_buffer) {
    OpenSwooleWG.output_buffer = (String **) osw_malloc(sizeof(String *) * n_buffer);
    if (OpenSwooleWG.output_buffer == nullptr) {
        openswoole_error("malloc for OpenSwooleWG.output_buffer failed");
        return OSW_ERR;
    }

    OSW_LOOP_N(n_buffer) {
        OpenSwooleWG.output_buffer[i] = new String(OSW_BUFFER_SIZE_BIG);
        if (OpenSwooleWG.output_buffer[i] == nullptr) {
            openswoole_error("output_buffer init failed");
            return OSW_ERR;
        }
    }
    return OSW_OK;
}

static void ReactorProcess_free_output_buffer(size_t n_buffer) {
    OSW_LOOP_N(n_buffer) {
        delete OpenSwooleWG.output_buffer[i];
    }
    osw_free(OpenSwooleWG.output_buffer);
}

static int ReactorProcess_loop(ProcessPool *pool, Worker *worker) {
    Server *serv = (Server *) pool->ptr;

    OpenSwooleG.process_type = OSW_PROCESS_WORKER;
    OpenSwooleG.pid = getpid();

    OpenSwooleG.process_id = worker->id;
    if (serv->max_request > 0) {
        OpenSwooleWG.run_always = false;
    }
    OpenSwooleWG.max_request = serv->max_request;
    OpenSwooleWG.worker = worker;

    OpenSwooleTG.id = 0;
    if (worker->id == 0) {
        OpenSwooleTG.update_time = 1;
    }

    serv->init_worker(worker);

    // create reactor
    if (!OpenSwooleTG.reactor) {
        if (openswoole_event_init(0) < 0) {
            return OSW_ERR;
        }
    }

    Reactor *reactor = OpenSwooleTG.reactor;

    if (OpenSwooleTG.timer && OpenSwooleTG.timer->get_reactor() == nullptr) {
        OpenSwooleTG.timer->reinit(reactor);
    }

    size_t n_buffer = serv->get_all_worker_num();
    if (ReactorProcess_alloc_output_buffer(n_buffer)) {
        return OSW_ERR;
    }

    for (auto ls : serv->ports) {
#ifdef HAVE_REUSEPORT
        if (ls->is_stream() && serv->enable_reuse_port) {
            if (ReactorProcess_reuse_port(ls) < 0) {
                ReactorProcess_free_output_buffer(n_buffer);
                openswoole_event_free();
                return OSW_ERR;
            }
        }
#endif
        if (reactor->add(ls->socket, OSW_EVENT_READ) < 0) {
            return OSW_ERR;
        }
    }

    reactor->id = worker->id;
    reactor->ptr = serv;

#ifdef HAVE_SIGNALFD
    if (OpenSwooleG.use_signalfd) {
        openswoole_signalfd_setup(OpenSwooleTG.reactor);
    }
#endif

    reactor->max_socket = serv->get_max_connection();

    reactor->close = Server::close_connection;

    // set event handler
    // connect
    reactor->set_handler(OSW_FD_STREAM_SERVER, Server::accept_connection);
    // close
    reactor->default_error_handler = ReactorProcess_onClose;
    // pipe
    reactor->set_handler(OSW_FD_PIPE | OSW_EVENT_READ, ReactorProcess_onPipeRead);

    serv->store_listen_socket();

    if (worker->pipe_worker) {
        worker->pipe_worker->set_nonblock();
        worker->pipe_master->set_nonblock();
        if (reactor->add(worker->pipe_worker, OSW_EVENT_READ) < 0) {
            return OSW_ERR;
        }
        if (reactor->add(worker->pipe_master, OSW_EVENT_READ) < 0) {
            return OSW_ERR;
        }
    }

    // task workers
    if (serv->task_worker_num > 0) {
        if (serv->task_ipc_mode == Server::TASK_IPC_UNIXSOCK) {
            OSW_LOOP_N(serv->gs->task_workers.worker_num) {
                serv->gs->task_workers.workers[i].pipe_master->set_nonblock();
            }
        }
    }

    serv->init_reactor(reactor);

    // single server trigger onStart event
    if (Server_is_single(serv)) {
        if (serv->onStart) {
            serv->onStart(serv);
        }
    }

    /**
     * 1 second timer
     */
    if ((serv->master_timer = openswoole_timer_add(1000, true, Server::timer_callback, serv)) == nullptr) {
    _fail:
        ReactorProcess_free_output_buffer(n_buffer);
        openswoole_event_free();
        return OSW_ERR;
    }

    serv->worker_start_callback();

    /**
     * for heartbeat check
     */
    if (serv->heartbeat_check_interval > 0) {
        serv->heartbeat_timer =
            openswoole_timer_add((long) (serv->heartbeat_check_interval * 1000), true, ReactorProcess_onTimeout, reactor);
        if (serv->heartbeat_timer == nullptr) {
            goto _fail;
        }
    }

    int retval = reactor->wait(nullptr);

    /**
     * Close all connections
     */
    serv->foreach_connection([serv](Connection *conn) { serv->close(conn->session_id, true); });

    /**
     * call internal serv hooks
     */
    if (serv->hooks[Server::HOOK_WORKER_CLOSE]) {
        void *hook_args[2];
        hook_args[0] = serv;
        hook_args[1] = (void *) (uintptr_t) OpenSwooleG.process_id;
        serv->call_hook(Server::HOOK_WORKER_CLOSE, hook_args);
    }

    openswoole_event_free();
    serv->worker_stop_callback();
    ReactorProcess_free_output_buffer(n_buffer);

    return retval;
}

static int ReactorProcess_onClose(Reactor *reactor, Event *event) {
    int fd = event->fd;
    Server *serv = (Server *) reactor->ptr;
    Connection *conn = serv->get_connection(fd);
    if (conn == nullptr || conn->active == 0) {
        return OSW_ERR;
    }
    if (event->socket->removed) {
        return Server::close_connection(reactor, event->socket);
    }
    if (reactor->del(event->socket) == 0) {
        if (conn->close_queued) {
            return Server::close_connection(reactor, event->socket);
        } else {
            return serv->notify(conn, OSW_SERVER_EVENT_CLOSE) ? OSW_OK : OSW_ERR;
        }
    } else {
        return OSW_ERR;
    }
}

static void ReactorProcess_onTimeout(Timer *timer, TimerNode *tnode) {
    Reactor *reactor = (Reactor *) tnode->data;
    Server *serv = (Server *) reactor->ptr;
    Event notify_ev{};
    double now = microtime();

    notify_ev.type = OSW_FD_SESSION;

    serv->foreach_connection([serv, reactor, now, &notify_ev](Connection *conn) {
        if (serv->is_healthy_connection(now, conn)) {
            return;
        }
#ifdef OSW_USE_OPENSSL
        if (conn->socket->ssl && conn->socket->ssl_state != OSW_SSL_STATE_READY) {
            Server::close_connection(reactor, conn->socket);
            return;
        }
#endif
        if (serv->disable_notify || conn->close_force) {
            Server::close_connection(reactor, conn->socket);
            return;
        }
        conn->close_force = 1;
        notify_ev.fd = conn->fd;
        notify_ev.socket = conn->socket;
        notify_ev.reactor_id = conn->reactor_id;
        ReactorProcess_onClose(reactor, &notify_ev);
    });
}

#ifdef HAVE_REUSEPORT
static int ReactorProcess_reuse_port(ListenPort *ls) {
    ls->socket = openswoole::make_socket(
        ls->type, ls->is_dgram() ? OSW_FD_DGRAM_SERVER : OSW_FD_STREAM_SERVER, OSW_SOCK_CLOEXEC | OSW_SOCK_NONBLOCK);
    if (ls->socket->set_reuse_port() < 0) {
        ls->socket->free();
        return OSW_ERR;
    }
    if (ls->socket->bind(ls->host, &ls->port) < 0) {
        ls->socket->free();
        return OSW_ERR;
    }
    return ls->listen();
}
#endif

}  // namespace openswoole
