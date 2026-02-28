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
#include "openswoole_lock.h"
#include "openswoole_util.h"

#include <assert.h>

using openswoole::network::Address;
using openswoole::network::SendfileTask;
using openswoole::network::Socket;

openswoole::Server *g_server_instance = nullptr;

namespace openswoole {

static void Server_signal_handler(int sig);

TimerCallback Server::get_timeout_callback(ListenPort *port, Reactor *reactor, Connection *conn) {
    return [this, port, conn, reactor](Timer *, TimerNode *) {
        if (conn->protect) {
            return;
        }
        long ms = time<std::chrono::milliseconds>(true);
        if (ms - conn->socket->last_received_time < port->max_idle_time &&
            ms - conn->socket->last_sent_time < port->max_idle_time) {
            return;
        }
        if (disable_notify || conn->closed || conn->close_force) {
            close_connection(reactor, conn->socket);
            return;
        }
        conn->close_force = 1;
        Event _ev{};
        _ev.fd = conn->fd;
        _ev.socket = conn->socket;
        reactor->trigger_close_event(&_ev);
    };
}

void Server::disable_accept() {
    enable_accept_timer = openswoole_timer_add(
        OSW_ACCEPT_RETRY_TIME * 1000,
        false,
        [](Timer *timer, TimerNode *tnode) {
            Server *serv = (Server *) tnode->data;
            for (auto port : serv->ports) {
                if (port->is_dgram()) {
                    continue;
                }
                openswoole_event_add(port->socket, OSW_EVENT_READ);
            }
            serv->enable_accept_timer = nullptr;
        },
        this);

    if (enable_accept_timer == nullptr) {
        return;
    }

    for (auto port : ports) {
        if (port->is_dgram()) {
            continue;
        }
        openswoole_event_del(port->socket);
    }
}

void Server::close_port(bool only_stream_port) {
    for (auto port : ports) {
        if (only_stream_port && port->is_dgram()) {
            continue;
        }
        if (port->socket) {
            port->socket->free();
            port->socket = nullptr;
        }
    }
}

int Server::accept_connection(Reactor *reactor, Event *event) {
    Server *serv = (Server *) reactor->ptr;
    ListenPort *listen_host = serv->get_port_by_server_fd(event->fd);

    for (int i = 0; i < OSW_ACCEPT_MAX_COUNT; i++) {
        Socket *sock = event->socket->accept();
        if (sock == nullptr) {
            switch (errno) {
            case EAGAIN:
                return OSW_OK;
            case EINTR:
                continue;
            default:
                if (errno == EMFILE || errno == ENFILE) {
                    serv->disable_accept();
                }
                openswoole_sys_warning("accept() failed");
                return OSW_OK;
            }
        }

        openswoole_trace("[Master] Accept new connection. maxfd=%d|minfd=%d|reactor_id=%d|conn=%d",
                     serv->get_maxfd(),
                     serv->get_minfd(),
                     reactor->id,
                     sock->fd);

        // too many connection
        if (sock->fd >= (int) serv->max_connection) {
            openswoole_error_log(
                OSW_LOG_WARNING, OSW_ERROR_SERVER_TOO_MANY_SOCKET, "Too many connections [now: %d]", sock->fd);
            sock->free();
            serv->disable_accept();
            return OSW_OK;
        }

        // add to connection_list
        Connection *conn = serv->add_connection(listen_host, sock, event->fd);
        if (conn == nullptr) {
            sock->free();
            return OSW_OK;
        }
        sock->chunk_size = OSW_SEND_BUFFER_SIZE;

#ifdef OSW_USE_OPENSSL
        if (listen_host->ssl) {
            if (!listen_host->ssl_create(conn, sock)) {
                reactor->close(reactor, sock);
                return OSW_OK;
            }
        } else {
            sock->ssl = nullptr;
        }
#endif
        if (serv->single_thread) {
            if (serv->connection_incoming(reactor, conn) < 0) {
                reactor->close(reactor, sock);
                return OSW_OK;
            }
        } else {
            DataHead ev{};
            ev.type = OSW_SERVER_EVENT_INCOMING;
            ev.fd = conn->session_id;
            ev.reactor_id = conn->reactor_id;
            if (serv->send_to_reactor_thread((EventData *) &ev, sizeof(ev), conn->session_id) < 0) {
                reactor->close(reactor, sock);
                return OSW_OK;
            }
        }
    }

    return OSW_OK;
}

int Server::connection_incoming(Reactor *reactor, Connection *conn) {
    ListenPort *port = get_port_by_server_fd(conn->server_fd);
    if (port->max_idle_time > 0) {
        auto timeout_callback = get_timeout_callback(port, reactor, conn);
        conn->socket->recv_timeout_ = port->max_idle_time;
        conn->socket->recv_timer = openswoole_timer_add(port->max_idle_time * 1000, true, timeout_callback);
    }
#ifdef OSW_USE_OPENSSL
    if (conn->socket->ssl) {
        return reactor->add(conn->socket, OSW_EVENT_READ);
    }
#endif
    // delay receive, wait resume command
    if (!enable_delay_receive) {
        if (reactor->add(conn->socket, OSW_EVENT_READ) < 0) {
            return OSW_ERR;
        }
    }
    // notify worker process
    if (onConnect) {
        if (!notify(conn, OSW_SERVER_EVENT_CONNECT)) {
            return OSW_ERR;
        }
    }
    return OSW_OK;
}

#ifdef OSW_SUPPORT_DTLS
dtls::Session *Server::accept_dtls_connection(ListenPort *port, Address *sa) {
    dtls::Session *session = nullptr;
    Connection *conn = nullptr;

    Socket *sock = make_socket(port->type, OSW_FD_SESSION, OSW_SOCK_CLOEXEC | OSW_SOCK_NONBLOCK);
    if (!sock) {
        return nullptr;
    }

    int fd = sock->fd;
    sock->set_reuse_addr();
#ifdef HAVE_KQUEUE
    sock->set_reuse_port();
#endif

    switch (port->type) {
    case OSW_SOCK_UDP:
    case OSW_SOCK_UDP6:
        break;
    default:
        OPENSSL_assert(0);
        break;
    }

    if (sock->bind(port->socket->info) < 0) {
        openswoole_sys_warning("bind() failed");
        goto _cleanup;
    }
    if (sock->is_inet6()) {
        sock->set_option(IPPROTO_IPV6, IPV6_V6ONLY, 0);
    }
    if (sock->connect(sa) < 0) {
        openswoole_sys_warning("connect(%s:%d) failed", sa->get_addr(), sa->get_port());
        goto _cleanup;
    }

    memcpy(&sock->info, sa, sizeof(*sa));
    sock->chunk_size = OSW_SSL_BUFFER_SIZE;

    conn = add_connection(port, sock, port->socket->fd);
    if (conn == nullptr) {
        goto _cleanup;
    }

    session = new dtls::Session(sock, port->ssl_context);
    port->dtls_sessions->emplace(fd, session);

    if (!session->init()) {
        goto _cleanup;
    }

    return session;

_cleanup:
    if (conn) {
        *conn = {};
    }
    if (session) {
        delete session;
    }
    sock->free();
    return nullptr;
}
#endif

void Server::set_max_connection(uint32_t _max_connection) {
    if (connection_list != nullptr) {
        openswoole_warning("max_connection must be set before server create");
        return;
    }
    max_connection = _max_connection;
    if (max_connection == 0) {
        max_connection = OSW_MIN(OSW_MAX_CONNECTION, OpenSwooleG.max_sockets);
    } else if (max_connection > OSW_SESSION_LIST_SIZE) {
        max_connection = OSW_SESSION_LIST_SIZE;
        openswoole_warning("max_connection is exceed the OSW_SESSION_LIST_SIZE, it's reset to %u", OSW_SESSION_LIST_SIZE);
    }
    if (OpenSwooleG.max_sockets > 0 && max_connection > OpenSwooleG.max_sockets) {
        max_connection = OpenSwooleG.max_sockets;
        openswoole_warning("max_connection is exceed the maximum value, it's reset to %u", OpenSwooleG.max_sockets);
    }
}

int Server::start_check() {
    // disable notice when use OSW_DISPATCH_ROUND and OSW_DISPATCH_QUEUE
    if (is_process_mode()) {
        if (!is_support_unsafe_events()) {
            if (onConnect) {
                openswoole_warning("cannot set 'onConnect' event when using dispatch_mode=1/3/7");
                onConnect = nullptr;
            }
            if (onClose) {
                openswoole_warning("cannot set 'onClose' event when using dispatch_mode=1/3/7");
                onClose = nullptr;
            }
            if (onBufferFull) {
                openswoole_warning("cannot set 'onBufferFull' event when using dispatch_mode=1/3/7");
                onBufferFull = nullptr;
            }
            if (onBufferEmpty) {
                openswoole_warning("cannot set 'onBufferEmpty' event when using dispatch_mode=1/3/7");
                onBufferEmpty = nullptr;
            }
            disable_notify = 1;
        }
        if (!is_support_send_yield()) {
            send_yield = 0;
        }
    } else {
        max_queued_bytes = 0;
    }
    if (task_worker_num > 0) {
        if (onTask == nullptr) {
            openswoole_warning("onTask event callback must be set");
            return OSW_ERR;
        }
    }
    if (send_timeout > 0 && send_timeout < OSW_TIMER_MIN_SEC) {
        send_timeout = OSW_TIMER_MIN_SEC;
    }
    if (heartbeat_check_interval > 0) {
        for (auto ls : ports) {
            if (ls->heartbeat_idle_time == 0) {
                ls->heartbeat_idle_time = heartbeat_check_interval * 2;
            }
        }
    }
    for (auto ls : ports) {
        if (ls->protocol.package_max_length < OSW_BUFFER_MIN_SIZE) {
            ls->protocol.package_max_length = OSW_BUFFER_MIN_SIZE;
        }
        if (if_require_receive_callback(ls, onReceive != nullptr)) {
            openswoole_warning("require onReceive callback");
            return OSW_ERR;
        }
        if (if_require_packet_callback(ls, onPacket != nullptr)) {
            openswoole_warning("require onPacket callback");
            return OSW_ERR;
        }
        if (ls->heartbeat_idle_time > 0) {
            int expect_heartbeat_check_interval = ls->heartbeat_idle_time > 2 ? ls->heartbeat_idle_time / 2 : 1;
            if (heartbeat_check_interval == 0 || heartbeat_check_interval > expect_heartbeat_check_interval) {
                heartbeat_check_interval = expect_heartbeat_check_interval;
            }
        }
    }
#ifdef OSW_USE_OPENSSL
    /**
     * OpenSSL thread-safe
     */
    if (is_process_mode() && !single_thread) {
        openswoole_ssl_init_thread_safety();
    }
#endif

    return OSW_OK;
}

void Server::store_listen_socket() {
    for (auto ls : ports) {
        int sockfd = ls->socket->fd;
        // save server socket to connection_list
        connection_list[sockfd].fd = sockfd;
        connection_list[sockfd].socket = ls->socket;
        connection_list[sockfd].socket_type = ls->type;
        connection_list[sockfd].object = ls;
        connection_list[sockfd].info.assign(ls->type, ls->host, ls->port);
        if (sockfd >= 0) {
            set_minfd(sockfd);
            set_maxfd(sockfd);
        }
    }
}

/**
 * only the memory of the Worker structure is allocated, no process is fork
 */
int Server::create_task_workers() {
    key_t key = 0;
    swIPCMode ipc_mode;

    if (task_ipc_mode == TASK_IPC_MSGQUEUE || task_ipc_mode == TASK_IPC_PREEMPTIVE) {
        key = message_queue_key;
        ipc_mode = OSW_IPC_MSGQUEUE;
    } else if (task_ipc_mode == TASK_IPC_STREAM) {
        ipc_mode = OSW_IPC_SOCKET;
    } else {
        ipc_mode = OSW_IPC_UNIXSOCK;
    }

    ProcessPool *pool = &gs->task_workers;
    *pool = {};
    if (pool->create(task_worker_num, key, ipc_mode) < 0) {
        openswoole_warning("[Master] create task_workers failed");
        return OSW_ERR;
    }

    pool->set_max_request(task_max_request, task_max_request_grace);
    pool->set_start_id(worker_num);
    pool->set_type(OSW_PROCESS_TASKWORKER);

    if (ipc_mode == OSW_IPC_SOCKET) {
        char sockfile[sizeof(struct sockaddr_un)];
        snprintf(sockfile, sizeof(sockfile), "/tmp/openswoole.task.%d.sock", gs->master_pid);
        if (gs->task_workers.listen(sockfile, 2048) < 0) {
            return OSW_ERR;
        }
    }

    init_task_workers();

    return OSW_OK;
}

/**
 * @description:
 *  only the memory of the Worker structure is allocated, no process is fork.
 *  called when the manager process start.
 * @param Server
 * @return: OSW_OK|OSW_ERR
 */
int Server::create_user_workers() {
    user_workers = (Worker *) osw_shm_calloc(get_user_worker_num(), sizeof(Worker));
    if (user_workers == nullptr) {
        openswoole_sys_warning("gmalloc[server->user_workers] failed");
        return OSW_ERR;
    }
    return OSW_OK;
}

/**
 * [Master]
 */
void Server::create_worker(Worker *worker) {
    worker->lock = new Mutex(Mutex::PROCESS_SHARED);
}

void Server::destroy_worker(Worker *worker) {
    delete worker->lock;
    worker->lock = nullptr;
}

/**
 * [Worker]
 */
void Server::init_worker(Worker *worker) {
#ifdef HAVE_CPU_AFFINITY
    if (open_cpu_affinity) {
        cpu_set_t cpu_set;
        CPU_ZERO(&cpu_set);
        if (cpu_affinity_available_num) {
            CPU_SET(cpu_affinity_available[OpenSwooleG.process_id % cpu_affinity_available_num], &cpu_set);
        } else {
            CPU_SET(OpenSwooleG.process_id % OSW_CPU_NUM, &cpu_set);
        }
        if (openswoole_set_cpu_affinity(&cpu_set) < 0) {
            openswoole_sys_warning("openswoole_set_cpu_affinity() failed");
        }
    }
#endif
    // signal init
    worker_signal_init();

    if (max_request < 1) {
        OpenSwooleWG.run_always = true;
    } else {
        OpenSwooleWG.max_request = max_request;
        if (max_request_grace > 0) {
            OpenSwooleWG.max_request += openswoole_system_random(1, max_request_grace);
        }
    }

    worker->start_time = ::time(nullptr);
    worker->request_count = 0;
}

void Server::call_worker_start_callback(Worker *worker) {
    void *hook_args[2];
    hook_args[0] = this;
    hook_args[1] = (void *) (uintptr_t) worker->id;

    if (OpenSwooleG.hooks[OSW_GLOBAL_HOOK_BEFORE_WORKER_START]) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_BEFORE_WORKER_START, hook_args);
    }
    if (hooks[Server::HOOK_WORKER_START]) {
        call_hook(Server::HOOK_WORKER_START, hook_args);
    }
    if (onWorkerStart) {
        onWorkerStart(this, worker->id);
    }
}

int Server::start() {
    if (start_check() < 0) {
        return OSW_ERR;
    }
    if (OpenSwooleG.hooks[OSW_GLOBAL_HOOK_BEFORE_SERVER_START]) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_BEFORE_SERVER_START, this);
    }
    // cannot start 2 servers at the same time, please use process->exec.
    if (!osw_atomic_cmp_set(&gs->start, 0, 1)) {
        openswoole_error_log(OSW_LOG_ERROR, OSW_ERROR_SERVER_ONLY_START_ONE, "can only start one server");
        return OSW_ERR;
    }
    // run as daemon
    if (daemonize > 0) {
        /**
         * redirect STDOUT to log file
         */
        if (osw_logger()->is_opened()) {
            osw_logger()->redirect_stdout_and_stderr(1);
        }
        /**
         * redirect STDOUT_FILENO/STDERR_FILENO to /dev/null
         */
        else {
            null_fd = open("/dev/null", O_WRONLY);
            if (null_fd > 0) {
                openswoole_redirect_stdout(null_fd);
            } else {
                openswoole_sys_warning("open(/dev/null) failed");
            }
        }

        if (openswoole_daemon(0, 1) < 0) {
            return OSW_ERR;
        }
    }

    // master pid
    gs->master_pid = getpid();
    gs->start_time = ::time(nullptr);

    gs->tasking_num = 0;
    gs->reload_count = 0;
    gs->reload_last_time = ::time(nullptr);

    /**
     * store to ProcessPool object
     */
    gs->event_workers.ptr = this;
    gs->event_workers.workers = workers;
    gs->event_workers.worker_num = worker_num;
    gs->event_workers.use_msgqueue = 0;

    OSW_LOOP_N(worker_num) {
        gs->event_workers.workers[i].pool = &gs->event_workers;
        gs->event_workers.workers[i].id = i;
        gs->event_workers.workers[i].type = OSW_PROCESS_WORKER;
    }

    /*
     * For openswoole_server->taskwait, create notify pipe and result shared memory.
     */
    if (task_worker_num > 0 && worker_num > 0) {
        task_result = (EventData *) osw_shm_calloc(worker_num, sizeof(EventData));
        if (!task_result) {
            openswoole_warning("malloc[task_result] failed");
            return OSW_ERR;
        }
        OSW_LOOP_N(worker_num) {
            auto _pipe = new Pipe(true);
            if (!_pipe->ready()) {
                osw_shm_free(task_result);
                delete _pipe;
                return OSW_ERR;
            }
            task_notify_pipes.emplace_back(_pipe);
        }
    }

    if (!user_worker_list.empty()) {
        uint32_t i = 0;
        for (auto worker : user_worker_list) {
            worker->id = worker_num + task_worker_num + i;
            i++;
        }
    }

    running = true;
    // factory start
    if (!factory->start()) {
        return OSW_ERR;
    }
    init_signal_handler();

    // write PID file
    if (!pid_file.empty()) {
        size_t n = osw_snprintf(osw_tg_buffer()->str, osw_tg_buffer()->size, "%d", getpid());
        file_put_contents(pid_file, osw_tg_buffer()->str, n);
    }
    int ret;
    if (is_base_mode()) {
        ret = start_reactor_processes();
    } else {
        ret = start_reactor_threads();
    }
    // failed to start
    if (ret < 0) {
        return OSW_ERR;
    }
    destroy();
    // remove PID file
    if (!pid_file.empty()) {
        unlink(pid_file.c_str());
    }
    return OSW_OK;
}

/**
 * initializing server config, set default
 */
Server::Server(enum Mode _mode) {
    openswoole_init();

    reactor_num = OSW_CPU_NUM > OSW_REACTOR_MAX_THREAD ? OSW_REACTOR_MAX_THREAD : OSW_CPU_NUM;
    worker_num = OSW_CPU_NUM;
    max_connection = OSW_MIN(OSW_MAX_CONNECTION, OpenSwooleG.max_sockets);
    mode_ = _mode;

    // http server
#ifdef OSW_HAVE_COMPRESSION
    http_compression = 1;
    http_compression_level = OSW_Z_BEST_SPEED;
    compression_min_length = OSW_COMPRESSION_MIN_LENGTH_DEFAULT;
#endif

#ifdef __linux__
    timezone_ = timezone;
#else
    struct timezone tz;
    struct timeval tv;
    gettimeofday(&tv, &tz);
    timezone_ = tz.tz_minuteswest * 60;
#endif

    /**
     * alloc shared memory
     */
    gs = (ServerGS *) osw_shm_malloc(sizeof(ServerGS));
    if (gs == nullptr) {
        openswoole_error("[Master] Fatal Error: failed to allocate memory for Server->gs");
    }

    worker_msg_id = 1;
    worker_buffer_allocator = osw_std_allocator();

    g_server_instance = this;
}

Server::~Server() {
    if (!is_shutdown() && getpid() == gs->master_pid) {
        destroy();
    }
    for (auto port : ports) {
        delete port;
    }
    osw_shm_free(gs);
}

int Server::create() {
    if (factory) {
        return OSW_ERR;
    }

    if (openswoole_isset_hook(OSW_GLOBAL_HOOK_BEFORE_SERVER_CREATE)) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_BEFORE_SERVER_CREATE, this);
    }

    session_list = (Session *) osw_shm_calloc(OSW_SESSION_LIST_SIZE, sizeof(Session));
    if (session_list == nullptr) {
        openswoole_error("osw_shm_calloc(%ld) for session_list failed", OSW_SESSION_LIST_SIZE * sizeof(Session));
        return OSW_ERR;
    }

    port_connnection_num_list = (uint32_t *) osw_shm_calloc(ports.size(), sizeof(osw_atomic_t));
    if (port_connnection_num_list == nullptr) {
        openswoole_error("osw_shm_calloc() for port_connnection_num_array failed");
        return OSW_ERR;
    }

    int index = 0;
    for (auto port : ports) {
        port->connection_num = &port_connnection_num_list[index++];
    }

    if (enable_static_handler and locations == nullptr) {
        locations = new std::unordered_set<std::string>;
    }

    // Max Connections
    uint32_t minimum_connection = (worker_num + task_worker_num) * 2 + 32;
    if (ports.size() > 0) {
        minimum_connection += ports.back()->get_fd();
    }
    if (max_connection < minimum_connection) {
        max_connection = OpenSwooleG.max_sockets;
        openswoole_warning(
            "max_connection must be bigger than %u, it's reset to %u", minimum_connection, OpenSwooleG.max_sockets);
    }
    // Reactor Thread Num
    if (reactor_num > OSW_CPU_NUM * OSW_MAX_THREAD_NCPU) {
        openswoole_warning("serv->reactor_num == %d, Too many threads, reset to max value %d",
                       reactor_num,
                       OSW_CPU_NUM * OSW_MAX_THREAD_NCPU);
        reactor_num = OSW_CPU_NUM * OSW_MAX_THREAD_NCPU;
    } else if (reactor_num == 0) {
        reactor_num = OSW_CPU_NUM;
    }
    if (single_thread) {
        reactor_num = 1;
    }
    // Worker Process Num
    if (worker_num > OSW_CPU_NUM * OSW_MAX_WORKER_NCPU) {
        openswoole_warning(
            "worker_num == %d, Too many processes, reset to max value %d", worker_num, OSW_CPU_NUM * OSW_MAX_WORKER_NCPU);
        worker_num = OSW_CPU_NUM * OSW_MAX_WORKER_NCPU;
    }
    if (worker_num < reactor_num) {
        reactor_num = worker_num;
    }
    // TaskWorker Process Num
    if (task_worker_num > 0) {
        if (task_worker_num > OSW_CPU_NUM * OSW_MAX_WORKER_NCPU) {
            openswoole_warning("serv->task_worker_num == %d, Too many processes, reset to max value %d",
                           task_worker_num,
                           OSW_CPU_NUM * OSW_MAX_WORKER_NCPU);
            task_worker_num = OSW_CPU_NUM * OSW_MAX_WORKER_NCPU;
        }
    }
    workers = (Worker *) osw_shm_calloc(worker_num, sizeof(Worker));
    if (workers == nullptr) {
        openswoole_sys_warning("gmalloc[server->workers] failed");
        return OSW_ERR;
    }

    int retval;
    if (is_base_mode()) {
        factory = new BaseFactory(this);
        retval = create_reactor_processes();
    } else {
        factory = new ProcessFactory(this);
        retval = create_reactor_threads();
    }

    if (openswoole_isset_hook(OSW_GLOBAL_HOOK_AFTER_SERVER_CREATE)) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_AFTER_SERVER_CREATE, this);
    }

    return retval;
}

void Server::clear_timer() {
    if (OpenSwooleTG.timer) {
        size_t num = OpenSwooleTG.timer->count(), index = 0;
        TimerNode **list = (TimerNode **) malloc(num * sizeof(TimerNode *));
        for (auto &kv : OpenSwooleTG.timer->get_map()) {
            TimerNode *tnode = kv.second;
            if (tnode->type == TimerNode::TYPE_PHP) {
                list[index++] = tnode;
            }
        }

        while (index--) {
            openswoole_timer_del(list[index]);
        }
        free(list);
    }
    if (master_timer) {
        openswoole_timer_del(master_timer);
        master_timer = nullptr;
    }
    if (heartbeat_timer) {
        openswoole_timer_del(heartbeat_timer);
        heartbeat_timer = nullptr;
    }
    if (enable_accept_timer) {
        openswoole_timer_del(enable_accept_timer);
        enable_accept_timer = nullptr;
    }
    if (OpenSwooleWG.event_loop_lag_timer) {
        openswoole_timer_del(OpenSwooleWG.event_loop_lag_timer);
        OpenSwooleWG.event_loop_lag_timer = nullptr;
    }
    if (single_thread && reactor_threads && reactor_threads[0].lag_timer) {
        openswoole_timer_del(reactor_threads[0].lag_timer);
        reactor_threads[0].lag_timer = nullptr;
    }
}

void Server::shutdown() {
    openswoole_trace_log(OSW_TRACE_SERVER, "shutdown service");
    if (openswoole_isset_hook(OSW_GLOBAL_HOOK_BEFORE_SERVER_SHUTDOWN)) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_BEFORE_SERVER_SHUTDOWN, this);
    }
    if (getpid() != gs->master_pid) {
        kill(gs->master_pid, SIGTERM);
        return;
    }
    running = false;
    // stop all thread
    if (OpenSwooleTG.reactor) {
        Reactor *reactor = OpenSwooleTG.reactor;
        reactor->set_wait_exit(true);
        for (auto port : ports) {
            if (port->is_dgram() and is_process_mode()) {
                continue;
            }
            reactor->del(port->socket);
        }
        clear_timer();
    }

    if (is_base_mode()) {
        gs->event_workers.running = 0;
    }

    openswoole_info("Server is shutdown now");
}

void Server::destroy() {
    openswoole_trace_log(OSW_TRACE_SERVER, "release service");
    if (openswoole_isset_hook(OSW_GLOBAL_HOOK_AFTER_SERVER_SHUTDOWN)) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_AFTER_SERVER_SHUTDOWN, this);
    }
    /**
     * shutdown workers
     */
    factory->shutdown();
    if (is_base_mode()) {
        openswoole_trace_log(OSW_TRACE_SERVER, "terminate task workers");
        if (task_worker_num > 0) {
            gs->task_workers.shutdown();
            gs->task_workers.destroy();
        }
    } else {
        openswoole_trace_log(OSW_TRACE_SERVER, "terminate reactor threads");
        /**
         * Wait until all the end of the thread
         */
        join_reactor_thread();
    }

    for (auto port : ports) {
        port->close();
    }

    if (user_workers) {
        osw_shm_free(user_workers);
        user_workers = nullptr;
    }
    if (null_fd > 0) {
        ::close(null_fd);
        null_fd = -1;
    }
    openswoole_signal_clear();
    /**
     * shutdown status
     */
    gs->start = 0;
    gs->shutdown = 1;
    /**
     * callback
     */
    if (onShutdown) {
        onShutdown(this);
    }
    if (is_base_mode()) {
        destroy_reactor_processes();
    } else {
        destroy_reactor_threads();
    }
    if (locations) {
        delete locations;
    }
    if (http_index_files) {
        delete http_index_files;
    }
    for (auto i = 0; i < OSW_MAX_HOOK_TYPE; i++) {
        if (hooks[i]) {
            std::list<Callback> *l = reinterpret_cast<std::list<Callback> *>(hooks[i]);
            hooks[i] = nullptr;
            delete l;
        }
    }

    osw_shm_free(session_list);
    osw_shm_free(port_connnection_num_list);
    osw_shm_free(workers);

    session_list = nullptr;
    port_connnection_num_list = nullptr;
    workers = nullptr;

    delete factory;
    factory = nullptr;

    g_server_instance = nullptr;
}

/**
 * worker to master process
 */
bool Server::feedback(Connection *conn, enum ServerEventType event) {
    SendData _send{};
    _send.info.type = event;
    _send.info.fd = conn->session_id;
    _send.info.reactor_id = conn->reactor_id;

    if (is_process_mode()) {
        return send_to_reactor_thread((EventData *) &_send.info, sizeof(_send.info), conn->session_id) > 0;
    } else {
        return send_to_connection(&_send) == OSW_OK;
    }
}

void Server::store_pipe_fd(UnixSocket *p) {
    Socket *master_socket = p->get_socket(true);
    Socket *worker_socket = p->get_socket(false);

    connection_list[master_socket->fd].object = p;
    connection_list[worker_socket->fd].object = p;

    if (master_socket->fd > get_maxfd()) {
        set_maxfd(master_socket->fd);
    }
    if (worker_socket->fd > get_maxfd()) {
        set_maxfd(worker_socket->fd);
    }
}

/**
 * @process Worker
 */
bool Server::send(SessionId session_id, const void *data, uint32_t length) {
    SendData _send{};
    _send.info.fd = session_id;
    _send.info.type = OSW_SERVER_EVENT_RECV_DATA;
    _send.data = (char *) data;
    _send.info.len = length;
    return factory->finish(&_send);
}

int Server::schedule_worker(int fd, SendData *data) {
    uint32_t key = 0;

    if (dispatch_func) {
        int id = dispatch_func(this, get_connection(fd), data);
        if (id != DISPATCH_RESULT_USERFUNC_FALLBACK) {
            return id;
        }
    }

    // polling mode
    if (dispatch_mode == DISPATCH_ROUND) {
        key = osw_atomic_fetch_add(&worker_round_id, 1);
    }
    // Using the FD touch access to hash
    else if (dispatch_mode == DISPATCH_FDMOD) {
        key = fd;
    }
    // Using the IP touch access to hash
    else if (dispatch_mode == DISPATCH_IPMOD) {
        Connection *conn = get_connection(fd);
        // UDP
        if (conn == nullptr) {
            key = fd;
        }
        // IPv4
        else if (conn->socket_type == OSW_SOCK_TCP) {
            key = conn->info.addr.inet_v4.sin_addr.s_addr;
        }
        // IPv6
        else {
#ifdef HAVE_KQUEUE
            key = *(((uint32_t *) &conn->info.addr.inet_v6.sin6_addr) + 3);
#elif defined(_WIN32)
            key = conn->info.addr.inet_v6.sin6_addr.u.Word[3];
#else
            key = conn->info.addr.inet_v6.sin6_addr.s6_addr32[3];
#endif
        }
    } else if (dispatch_mode == DISPATCH_UIDMOD) {
        Connection *conn = get_connection(fd);
        if (conn == nullptr || conn->uid == 0) {
            key = fd;
        } else {
            key = conn->uid;
        }
    } else if (dispatch_mode == DISPATCH_CO_CONN_LB) {
        Connection *conn = get_connection(fd);
        // if (conn == nullptr) {
        //     return key % worker_num;
        // }
        if (conn == nullptr || conn->worker_id < 0) {
            conn->worker_id = get_lowest_load_worker_id();
        }
        return conn->worker_id;
    } else if (dispatch_mode == DISPATCH_CO_REQ_LB) {
        return get_lowest_load_worker_id();
    }
    // deliver tasks to idle worker processes
    else {
        uint32_t i;
        bool found = false;
        for (i = 0; i < worker_num + 1; i++) {
            key = osw_atomic_fetch_add(&worker_round_id, 1) % worker_num;
            if (workers[key].status == OSW_WORKER_IDLE) {
                found = true;
                break;
            }
        }
        if (osw_unlikely(!found)) {
            scheduler_warning = true;
        }
        openswoole_trace_log(OSW_TRACE_SERVER, "schedule=%d, round=%d", key, worker_round_id);
        return key;
    }

    return key % worker_num;
}

/**
 * [Master] send to client or append to out_buffer
 * @return OSW_OK or OSW_ERR
 */
int Server::send_to_connection(SendData *_send) {
    SessionId session_id = _send->info.fd;
    const char *_send_data = _send->data;
    uint32_t _send_length = _send->info.len;

    Connection *conn;
    if (_send->info.type != OSW_SERVER_EVENT_CLOSE) {
        conn = get_connection_verify(session_id);
    } else {
        conn = get_connection_verify_no_ssl(session_id);
    }
    if (!conn) {
        if (_send->info.type == OSW_SERVER_EVENT_RECV_DATA) {
            openswoole_error_log(OSW_LOG_NOTICE,
                             OSW_ERROR_SESSION_NOT_EXIST,
                             "send %d byte failed, session#%ld does not exist",
                             _send_length,
                             session_id);
        } else {
            openswoole_error_log(OSW_LOG_NOTICE,
                             OSW_ERROR_SESSION_NOT_EXIST,
                             "send event[%d] failed, session#%ld does not exist",
                             _send->info.type,
                             session_id);
        }
        return OSW_ERR;
    }

    int fd = conn->fd;
    Reactor *reactor = OpenSwooleTG.reactor;
    ListenPort *port = get_port_by_server_fd(conn->server_fd);

    if (!single_thread) {
        assert(fd % reactor_num == reactor->id);
        assert(fd % reactor_num == OpenSwooleTG.id);
    }

    if (is_base_mode() && conn->overflow) {
        if (send_yield) {
            openswoole_set_last_error(OSW_ERROR_OUTPUT_SEND_YIELD);
        } else {
            openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_OUTPUT_BUFFER_OVERFLOW, "socket#%d output buffer overflow", fd);
        }
        return OSW_ERR;
    }

    Socket *_socket = conn->socket;

    /**
     * Reset send buffer, Immediately close the connection.
     */
    if (_send->info.type == OSW_SERVER_EVENT_CLOSE && (conn->close_reset || conn->close_force || conn->peer_closed)) {
        goto _close_fd;
    }
    /**
     * pause recv data
     */
    else if (_send->info.type == OSW_SERVER_EVENT_PAUSE_RECV) {
        if (_socket->removed || !(_socket->events & OSW_EVENT_READ)) {
            return OSW_OK;
        }
        if (_socket->events & OSW_EVENT_WRITE) {
            return reactor->set(conn->socket, OSW_EVENT_WRITE);
        } else {
            return reactor->del(conn->socket);
        }
    }
    /**
     * resume recv data
     */
    else if (_send->info.type == OSW_SERVER_EVENT_RESUME_RECV) {
        if (!_socket->removed || (_socket->events & OSW_EVENT_READ)) {
            return OSW_OK;
        }
        if (_socket->events & OSW_EVENT_WRITE) {
            return reactor->set(_socket, OSW_EVENT_READ | OSW_EVENT_WRITE);
        } else {
            return reactor->add(_socket, OSW_EVENT_READ);
        }
    }

    if (Buffer::empty(_socket->out_buffer)) {
        /**
         * close connection.
         */
        if (_send->info.type == OSW_SERVER_EVENT_CLOSE) {
        _close_fd:
            reactor->close(reactor, _socket);
            return OSW_OK;
        }
        // Direct send
        if (_send->info.type != OSW_SERVER_EVENT_SEND_FILE) {
            if (!_socket->direct_send) {
                goto _buffer_send;
            }

            ssize_t n;

        _direct_send:
            n = _socket->send(_send_data, _send_length, 0);
            if (n == _send_length) {
                conn->last_send_time = microtime();
                return OSW_OK;
            } else if (n > 0) {
                _send_data += n;
                _send_length -= n;
                goto _buffer_send;
            } else if (errno == EINTR) {
                goto _direct_send;
            } else {
                goto _buffer_send;
            }
        }
        // buffer send
        else {
        _buffer_send:
            if (!_socket->out_buffer) {
                _socket->out_buffer = new Buffer(OSW_SEND_BUFFER_SIZE);
            }
        }
    }

    BufferChunk *chunk;
    // close connection
    if (_send->info.type == OSW_SERVER_EVENT_CLOSE) {
        chunk = _socket->out_buffer->alloc(BufferChunk::TYPE_CLOSE, 0);
        chunk->value.data.val1 = _send->info.type;
        conn->close_queued = 1;
    }
    // sendfile to client
    else if (_send->info.type == OSW_SERVER_EVENT_SEND_FILE) {
        SendfileTask *task = (SendfileTask *) _send_data;
        if (conn->socket->sendfile(task->filename, task->offset, task->length) < 0) {
            return false;
        }
    }
    // send data
    else {
        // connection is closed
        if (conn->peer_closed) {
            openswoole_error_log(
                OSW_LOG_NOTICE, OSW_ERROR_SESSION_CLOSED_BY_CLIENT, "Server::send(): socket#%d is closed by client", fd);
            return false;
        }
        // connection output buffer overflow
        if (_socket->out_buffer->length() >= _socket->buffer_size) {
            if (send_yield) {
                openswoole_set_last_error(OSW_ERROR_OUTPUT_SEND_YIELD);
            } else {
                openswoole_error_log(OSW_LOG_WARNING,
                                 OSW_ERROR_OUTPUT_BUFFER_OVERFLOW,
                                 "Server::send(): connection#%d output buffer overflow",
                                 fd);
            }
            conn->overflow = 1;
            if (onBufferEmpty && onBufferFull == nullptr) {
                conn->high_watermark = 1;
            }
        }

        _socket->out_buffer->append(_send_data, _send_length);
        conn->send_queued_bytes = _socket->out_buffer->length();

        ListenPort *port = get_port_by_fd(fd);
        if (onBufferFull && conn->high_watermark == 0 && _socket->out_buffer->length() >= port->buffer_high_watermark) {
            notify(conn, OSW_SERVER_EVENT_BUFFER_FULL);
            conn->high_watermark = 1;
        }
    }

    if (port->max_idle_time > 0 && _socket->send_timer == nullptr) {
        auto timeout_callback = get_timeout_callback(port, reactor, conn);
        _socket->send_timeout_ = port->max_idle_time;
        _socket->last_sent_time = time<std::chrono::milliseconds>(true);
        _socket->send_timer = openswoole_timer_add(port->max_idle_time * 1000, true, timeout_callback);
    }

    if (!_socket->isset_writable_event()) {
        reactor->add_write_event(_socket);
    }

    return OSW_OK;
}

/**
 * use in master process
 */
bool Server::notify(Connection *conn, enum ServerEventType event) {
    DataHead notify_event = {};
    notify_event.type = event;
    notify_event.reactor_id = conn->reactor_id;
    notify_event.fd = conn->fd;
    notify_event.server_fd = conn->server_fd;
    return factory->notify(&notify_event);
}

/**
 * @process Worker
 */
bool Server::sendfile(SessionId session_id, const char *file, uint32_t l_file, off_t offset, size_t length) {
    if (osw_unlikely(session_id <= 0)) {
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SESSION_INVALID_ID, "invalid fd[%ld]", session_id);
        return false;
    }

    if (osw_unlikely(is_master())) {
        openswoole_error_log(
            OSW_LOG_ERROR, OSW_ERROR_SERVER_SEND_IN_MASTER, "can't send data to the connections in master process");
        return false;
    }

    char _buffer[OSW_IPC_BUFFER_SIZE];
    SendfileTask *req = reinterpret_cast<SendfileTask *>(_buffer);

    // file name size
    if (osw_unlikely(l_file > sizeof(_buffer) - sizeof(*req) - 1)) {
        openswoole_error_log(OSW_LOG_WARNING,
                         OSW_ERROR_NAME_TOO_LONG,
                         "sendfile name[%.8s...] length %u is exceed the max name len %u",
                         file,
                         l_file,
                         (uint32_t) (OSW_IPC_BUFFER_SIZE - sizeof(SendfileTask) - 1));
        return false;
    }
    // string must be zero termination (for `state` system call)
    openswoole_strlcpy((char *) req->filename, file, sizeof(_buffer) - sizeof(*req));

    // check state
    struct stat file_stat;
    if (stat(req->filename, &file_stat) < 0) {
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SYSTEM_CALL_FAIL, "stat(%s) failed", req->filename);
        return false;
    }
    if (file_stat.st_size <= offset) {
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SYSTEM_CALL_FAIL, "file[offset=%ld] is empty", (long) offset);
        return false;
    }
    req->offset = offset;
    req->length = length;

    // construct send data
    SendData send_data{};
    send_data.info.fd = session_id;
    send_data.info.type = OSW_SERVER_EVENT_SEND_FILE;
    send_data.info.len = sizeof(SendfileTask) + l_file + 1;
    send_data.data = _buffer;

    return factory->finish(&send_data);
}

/**
 * [Worker] Returns the number of bytes sent
 */
bool Server::sendwait(SessionId session_id, const void *data, uint32_t length) {
    Connection *conn = get_connection_verify(session_id);
    if (!conn) {
        openswoole_error_log(OSW_LOG_NOTICE,
                         OSW_ERROR_SESSION_CLOSED,
                         "send %d byte failed, because session#%ld is closed",
                         length,
                         session_id);
        return false;
    }
    return conn->socket->send_blocking(data, length) == length;
}

size_t Server::get_packet(EventData *req, char **data_ptr) {
    size_t length;
    if (req->info.flags & OSW_EVENT_DATA_PTR) {
        PacketPtr *task = (PacketPtr *) req;
        *data_ptr = task->data.str;
        length = task->data.length;
    } else if (req->info.flags & OSW_EVENT_DATA_OBJ_PTR) {
        String *worker_buffer;
        memcpy(&worker_buffer, req->data, sizeof(worker_buffer));
        *data_ptr = worker_buffer->str;
        length = worker_buffer->length;
    } else {
        *data_ptr = req->data;
        length = req->info.len;
    }

    return length;
}

void Server::call_hook(HookType type, void *arg) {
    openswoole::hook_call(hooks, type, arg);
}

/**
 * [Worker]
 */
bool Server::close(SessionId session_id, bool reset) {
    return factory->end(session_id, reset ? CLOSE_ACTIVELY | CLOSE_RESET : CLOSE_ACTIVELY);
}

void Server::init_signal_handler() {
    openswoole_signal_set(SIGPIPE, nullptr);
    openswoole_signal_set(SIGHUP, nullptr);
    if (is_process_mode()) {
        openswoole_signal_set(SIGCHLD, Server_signal_handler);
    } else {
        openswoole_signal_set(SIGIO, Server_signal_handler);
    }
    openswoole_signal_set(SIGUSR1, Server_signal_handler);
    openswoole_signal_set(SIGUSR2, Server_signal_handler);
    openswoole_signal_set(SIGTERM, Server_signal_handler);
#ifdef SIGRTMIN
    openswoole_signal_set(SIGRTMIN, Server_signal_handler);
#endif
    // for test
    openswoole_signal_set(SIGVTALRM, Server_signal_handler);

    set_minfd(OpenSwooleG.signal_fd);
}

void Server::timer_callback(Timer *timer, TimerNode *tnode) {
    Server *serv = (Server *) tnode->data;
    time_t now = ::time(nullptr);
    if (serv->scheduler_warning && serv->warning_time < now) {
        serv->scheduler_warning = false;
        serv->warning_time = now;
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SERVER_NO_IDLE_WORKER, "No idle worker is available");
    }

    if (serv->gs->task_workers.scheduler_warning && serv->gs->task_workers.warning_time < now) {
        serv->gs->task_workers.scheduler_warning = 0;
        serv->gs->task_workers.warning_time = now;
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SERVER_NO_IDLE_WORKER, "No idle task worker is available");
    }

    if (serv->hooks[Server::HOOK_MASTER_TIMER]) {
        serv->call_hook(Server::HOOK_MASTER_TIMER, serv);
    }
}

int Server::add_worker(Worker *worker) {
    user_worker_list.push_back(worker);
    return worker->id;
}

int Server::add_hook(Server::HookType type, const Callback &func, int push_back) {
    return openswoole::hook_add(hooks, (int) type, func, push_back);
}

void Server::check_port_type(ListenPort *ls) {
    if (ls->is_dgram()) {
        // dgram socket, setting socket buffer size
        ls->socket->set_buffer_size(ls->socket_buffer_size);
        have_dgram_sock = 1;
        dgram_port_num++;
        if (ls->type == OSW_SOCK_UDP) {
            udp_socket_ipv4 = ls->socket;
        } else if (ls->type == OSW_SOCK_UDP6) {
            udp_socket_ipv6 = ls->socket;
        } else if (ls->type == OSW_SOCK_UNIX_DGRAM) {
            dgram_socket = ls->socket;
        }
    } else {
        have_stream_sock = 1;
    }
}

bool Server::is_healthy_connection(double now, Connection *conn) {
    if (conn->protect || conn->last_recv_time == 0) {
        return true;
    }
    auto lp = get_port_by_session_id(conn->session_id);
    if (!lp) {
        return true;
    }
    if (lp->heartbeat_idle_time == 0) {
        return true;
    }
    if (conn->last_recv_time > now - lp->heartbeat_idle_time) {
        return true;
    }
    return false;
}

/**
 * Return the number of ports successfully
 */
int Server::add_systemd_socket() {
    int pid;
    if (!openswoole_get_env("LISTEN_PID", &pid) && getpid() != pid) {
        openswoole_warning("invalid LISTEN_PID");
        return 0;
    }

    int n = openswoole_get_systemd_listen_fds();
    if (n <= 0) {
        return 0;
    }

    int count = 0;
    int sock;

    int start_fd;
    if (!openswoole_get_env("LISTEN_FDS_START", &start_fd)) {
        start_fd = OSW_SYSTEMD_FDS_START;
    } else if (start_fd < 0) {
        openswoole_warning("invalid LISTEN_FDS_START");
        return 0;
    }

    for (sock = start_fd; sock < start_fd + n; sock++) {
        std::unique_ptr<ListenPort> ptr(new ListenPort());
        ListenPort *ls = ptr.get();

        if (!ls->import(sock)) {
            continue;
        }

        // O_NONBLOCK & O_CLOEXEC
        ls->socket->set_fd_option(1, 1);

        ptr.release();
        check_port_type(ls);
        ports.push_back(ls);
        count++;
    }

    return count;
}

ListenPort *Server::add_port(SocketType type, const char *host, int port) {
    if (session_list) {
        openswoole_error_log(OSW_LOG_ERROR, OSW_ERROR_WRONG_OPERATION, "must add port before server is created");
        return nullptr;
    }
    if (ports.size() >= OSW_MAX_LISTEN_PORT) {
        openswoole_error_log(OSW_LOG_ERROR,
                         OSW_ERROR_SERVER_TOO_MANY_LISTEN_PORT,
                         "up to %d listening ports are allowed",
                         OSW_MAX_LISTEN_PORT);
        return nullptr;
    }
    if (!(type == OSW_SOCK_UNIX_DGRAM || type == OSW_SOCK_UNIX_STREAM) && (port < 0 || port > 65535)) {
        openswoole_error_log(OSW_LOG_ERROR, OSW_ERROR_SERVER_INVALID_LISTEN_PORT, "invalid port [%d]", port);
        return nullptr;
    }
    if (strlen(host) + 1 > OSW_HOST_MAXSIZE) {
        openswoole_error_log(OSW_LOG_ERROR,
                         OSW_ERROR_NAME_TOO_LONG,
                         "address '%s' exceeds the limit of %ld characters",
                         host,
                         OSW_HOST_MAXSIZE - 1);
        return nullptr;
    }

    std::unique_ptr<ListenPort> ptr(new ListenPort);
    ListenPort *ls = ptr.get();

    ls->type = type;
    ls->port = port;
    ls->host = host;

#ifdef OSW_USE_OPENSSL
    if (type & OSW_SOCK_SSL) {
        type = (SocketType) (type & (~OSW_SOCK_SSL));
        ls->type = type;
        ls->ssl = 1;
        ls->ssl_context = new SSLContext();
        ls->ssl_context->prefer_server_ciphers = 1;
        ls->ssl_context->session_tickets = 0;
        ls->ssl_context->stapling = 1;
        ls->ssl_context->stapling_verify = 1;
        ls->ssl_context->ciphers = osw_strdup(OSW_SSL_CIPHER_LIST);
        ls->ssl_context->ecdh_curve = osw_strdup(OSW_SSL_ECDH_CURVE);

        if (ls->is_dgram()) {
#ifdef OSW_SUPPORT_DTLS
            ls->ssl_context->protocols = OSW_SSL_DTLS;
            ls->dtls_sessions = new std::unordered_map<int, dtls::Session *>;

#else
            openswoole_warning("DTLS support require openssl-1.1 or later");
            return nullptr;
#endif
        }
    }
#endif

    ls->socket = make_socket(
        ls->type, ls->is_dgram() ? OSW_FD_DGRAM_SERVER : OSW_FD_STREAM_SERVER, OSW_SOCK_CLOEXEC | OSW_SOCK_NONBLOCK);
    if (ls->socket == nullptr) {
        return nullptr;
    }
#if defined(OSW_SUPPORT_DTLS) && defined(HAVE_KQUEUE)
    if (ls->is_dtls()) {
        ls->socket->set_reuse_port();
    }
#endif

    if (ls->socket->bind(ls->host, &ls->port) < 0) {
        ls->socket->free();
        return nullptr;
    }
    ls->socket->info.assign(ls->type, ls->host, ls->port);
    check_port_type(ls);
    ptr.release();
    ports.push_back(ls);
    return ls;
}

static void Server_signal_handler(int sig) {
    openswoole_trace_log(OSW_TRACE_SERVER, "signal[%d] %s triggered in %d", sig, openswoole_signal_to_str(sig), getpid());

    Server *serv = osw_server();
    if (!OpenSwooleG.running or !serv) {
        return;
    }

    int status;
    pid_t pid;
    switch (sig) {
    case SIGTERM:
        serv->shutdown();
        break;
    case SIGCHLD:
        if (!serv->running) {
            break;
        }
        if (osw_server()->is_base_mode()) {
            break;
        }
        pid = waitpid(-1, &status, WNOHANG);
        if (pid > 0 && pid == serv->gs->manager_pid) {
            openswoole_warning("Fatal Error: manager process exit. status=%d, signal=[%s]",
                           WEXITSTATUS(status),
                           openswoole_signal_to_str(WTERMSIG(status)));
        }
        break;
        /**
         * for test
         */
    case SIGVTALRM:
        openswoole_warning("SIGVTALRM coming");
        break;
        /**
         * proxy the restart signal
         */
    case SIGUSR1:
    case SIGUSR2:
        if (serv->is_base_mode()) {
            if (serv->gs->event_workers.reloading) {
                break;
            }
            serv->gs->event_workers.reloading = true;
            serv->gs->event_workers.reload_init = false;
        } else {
            openswoole_kill(serv->gs->manager_pid, sig);
        }
        osw_logger()->reopen();
        break;
    default:

#ifdef SIGRTMIN
        if (sig == SIGRTMIN) {
            uint32_t i;
            Worker *worker;
            for (i = 0; i < serv->worker_num + serv->task_worker_num + serv->get_user_worker_num(); i++) {
                worker = serv->get_worker(i);
                openswoole_kill(worker->pid, SIGRTMIN);
            }
            if (serv->is_process_mode()) {
                openswoole_kill(serv->gs->manager_pid, SIGRTMIN);
            }
            osw_logger()->reopen();
        }
#endif
        break;
    }
}

void Server::foreach_connection(const std::function<void(Connection *)> &callback) {
    for (int fd = get_minfd(); fd <= get_maxfd(); fd++) {
        Connection *conn = get_connection(fd);
        if (is_valid_connection(conn)) {
            callback(conn);
        }
    }
}

/**
 * new connection
 */
Connection *Server::add_connection(ListenPort *ls, Socket *_socket, int server_fd) {
    gs->accept_count++;
    osw_atomic_fetch_add(&gs->connection_num, 1);
    osw_atomic_fetch_add(ls->connection_num, 1);

    int fd = _socket->fd;

    lock();
    if (fd > get_maxfd()) {
        set_maxfd(fd);
    } else if (fd < get_minfd()) {
        set_minfd(fd);
    }
    unlock();

    Connection *connection = &(connection_list[fd]);
    ReactorId reactor_id = is_base_mode() ? OpenSwooleG.process_id : fd % reactor_num;
    *connection = {};

    osw_spinlock(&gs->spinlock);
    SessionId session_id = gs->session_round;
    // get session id
    OSW_LOOP_N(max_connection) {
        Session *session = get_session(++session_id);
        // available slot
        if (session->fd == 0) {
            session->fd = fd;
            session->id = session_id;
            session->reactor_id = reactor_id;
            goto _find_available_slot;
        }
    }
    osw_spinlock_release(&gs->spinlock);
    openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_SERVER_TOO_MANY_SOCKET, "no available session slot, fd=%d", fd);
    return nullptr;

_find_available_slot:
    osw_spinlock_release(&gs->spinlock);
    gs->session_round = session_id;
    connection->session_id = session_id;

    _socket->object = connection;
    _socket->removed = 1;
    _socket->buffer_size = ls->socket_buffer_size;
    _socket->send_timeout_ = _socket->recv_timeout_ = 0;

    // TCP Nodelay
    if (ls->open_tcp_nodelay && (ls->type == OSW_SOCK_TCP || ls->type == OSW_SOCK_TCP6)) {
        if (ls->socket->set_tcp_nodelay() != 0) {
            openswoole_sys_warning("setsockopt(TCP_NODELAY) failed");
        }
        _socket->enable_tcp_nodelay = true;
    }

    // socket recv buffer size
    if (ls->kernel_socket_recv_buffer_size > 0) {
        if (ls->socket->set_option(SOL_SOCKET, SO_RCVBUF, ls->kernel_socket_recv_buffer_size) != 0) {
            openswoole_sys_warning("setsockopt(SO_RCVBUF, %d) failed", ls->kernel_socket_recv_buffer_size);
        }
    }

    // socket send buffer size
    if (ls->kernel_socket_send_buffer_size > 0) {
        if (ls->socket->set_option(SOL_SOCKET, SO_SNDBUF, ls->kernel_socket_send_buffer_size) != 0) {
            openswoole_sys_warning("setsockopt(SO_SNDBUF, %d) failed", ls->kernel_socket_send_buffer_size);
        }
    }

    connection->fd = fd;
    connection->reactor_id = reactor_id;
    connection->server_fd = (osw_atomic_t) server_fd;
    connection->last_recv_time = connection->connect_time = microtime();
    connection->active = 1;
    connection->worker_id = -1;
    connection->socket_type = ls->type;
    connection->socket = _socket;

    memcpy(&connection->info.addr, &_socket->info.addr, _socket->info.len);
    connection->info.len = _socket->info.len;
    connection->info.type = connection->socket_type;

    if (!ls->ssl) {
        _socket->direct_send = 1;
    }

    return connection;
}

void Server::init_ipc_max_size() {
#ifdef HAVE_KQUEUE
    ipc_max_size = OSW_IPC_MAX_SIZE;
#else
    int bufsize;
    /**
     * Get the maximum ipc[unix socket with dgram] transmission length
     */
    if (workers[0].pipe_master->get_option(SOL_SOCKET, SO_SNDBUF, &bufsize) != 0) {
        bufsize = OSW_IPC_MAX_SIZE;
    }
    ipc_max_size = bufsize - OSW_DGRAM_HEADER_SIZE;
#endif
}

/**
 * allocate memory for Server::pipe_buffers
 */
int Server::create_pipe_buffers() {
    pipe_buffers = (PipeBuffer **) osw_calloc(reactor_num, sizeof(PipeBuffer *));
    if (pipe_buffers == nullptr) {
        openswoole_sys_error("malloc[buffers] failed");
        return OSW_ERR;
    }
    for (uint32_t i = 0; i < reactor_num; i++) {
        pipe_buffers[i] = (PipeBuffer *) osw_malloc(ipc_max_size);
        if (pipe_buffers[i] == nullptr) {
            openswoole_sys_error("malloc[sndbuf][%d] failed", i);
            return OSW_ERR;
        }
        osw_memset_zero(pipe_buffers[i], sizeof(DataHead));
    }

    return OSW_OK;
}

int Server::get_idle_worker_num() {
    uint32_t i;
    uint32_t idle_worker_num = 0;

    for (i = 0; i < worker_num; i++) {
        Worker *worker = get_worker(i);
        if (worker->status == OSW_WORKER_IDLE) {
            idle_worker_num++;
        }
    }

    return idle_worker_num;
}

int Server::get_idle_task_worker_num() {
    uint32_t i;
    uint32_t idle_worker_num = 0;

    for (i = worker_num; i < (worker_num + task_worker_num); i++) {
        Worker *worker = get_worker(i);
        if (worker->status == OSW_WORKER_IDLE) {
            idle_worker_num++;
        }
    }

    return idle_worker_num;
}
}  // namespace openswoole
