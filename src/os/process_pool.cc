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

#include "openswoole_api.h"
#include "openswoole_memory.h"
#include "openswoole_socket.h"
#include "openswoole_string.h"
#include "openswoole_msg_queue.h"
#include "openswoole_pipe.h"
#include "openswoole_server.h"
#include "openswoole_util.h"
#include "openswoole_process_pool.h"
#include "openswoole_client.h"

namespace openswoole {

using network::Socket;
using network::Stream;

/**
 * call onTask
 */
static int ProcessPool_worker_loop(ProcessPool *pool, Worker *worker);
/**
 * call onMessage
 */
static int ProcessPool_worker_loop_ex(ProcessPool *pool, Worker *worker);

void ProcessPool::kill_timeout_worker(Timer *timer, TimerNode *tnode) {
    uint32_t i;
    pid_t reload_worker_pid = 0;
    ProcessPool *pool = (ProcessPool *) tnode->data;
    pool->reloading = false;

    for (i = 0; i < pool->worker_num; i++) {
        if (i >= pool->reload_worker_i) {
            reload_worker_pid = pool->reload_workers[i].pid;
            if (openswoole_kill(reload_worker_pid, 0) == -1) {
                continue;
            }
            if (openswoole_kill(reload_worker_pid, SIGKILL) < 0) {
                openswoole_sys_warning(
                    "failed to force kill worker process(pid=%d, id=%d)", pool->reload_workers[i].pid, i);
            } else {
                openswoole_warning("force kill worker process(pid=%d, id=%d)", pool->reload_workers[i].pid, i);
            }
        }
    }
    errno = 0;
    pool->reload_worker_i = 0;
    pool->reload_init = false;
}
/**
 * Process manager
 */
int ProcessPool::create(uint32_t _worker_num, key_t _msgqueue_key, swIPCMode _ipc_mode) {
    worker_num = _worker_num;
    /**
     * Shared memory is used here
     */
    workers = (Worker *) osw_mem_pool()->alloc(_worker_num * sizeof(Worker));
    if (workers == nullptr) {
        openswoole_sys_warning("malloc[1] failed");
        return OSW_ERR;
    }

    message_box = Channel::make(65536, sizeof(WorkerStopMessage), OSW_CHAN_LOCK | OSW_CHAN_SHM);
    if (message_box == nullptr) {
        return OSW_ERR;
    }

    if (_ipc_mode == OSW_IPC_MSGQUEUE) {
        use_msgqueue = 1;
        msgqueue_key = _msgqueue_key;
        queue = new MsgQueue(msgqueue_key);
        if (!queue->ready()) {
            delete queue;
            queue = nullptr;
            return OSW_ERR;
        }
    } else if (_ipc_mode == OSW_IPC_UNIXSOCK) {
        pipes = new std::vector<std::shared_ptr<UnixSocket>>;
        OSW_LOOP_N(_worker_num) {
            auto sock = new UnixSocket(true, SOCK_DGRAM);
            if (!sock->ready()) {
                delete sock;
                delete pipes;
                pipes = nullptr;
                return OSW_ERR;
            }
            pipes->emplace_back(sock);
            workers[i].pipe_master = sock->get_socket(true);
            workers[i].pipe_worker = sock->get_socket(false);
            workers[i].pipe_object = sock;
        }
    } else if (_ipc_mode == OSW_IPC_SOCKET) {
        use_socket = 1;
        stream_info_ = new StreamInfo();
    } else {
        _ipc_mode = OSW_IPC_NONE;
    }

    map_ = new std::unordered_map<pid_t, Worker *>;

    ipc_mode = _ipc_mode;
    if (_ipc_mode > OSW_IPC_NONE) {
        main_loop = ProcessPool_worker_loop;
    }

    OSW_LOOP_N(_worker_num) {
        workers[i].pool = this;
    }

    return OSW_OK;
}

int ProcessPool::listen(const char *socket_file, int blacklog) {
    if (ipc_mode != OSW_IPC_SOCKET) {
        openswoole_warning("ipc_mode is not OSW_IPC_SOCKET");
        return OSW_ERR;
    }
    stream_info_->socket_file = osw_strdup(socket_file);
    if (stream_info_->socket_file == nullptr) {
        return OSW_ERR;
    }
    stream_info_->socket_port = 0;
    stream_info_->socket = make_server_socket(OSW_SOCK_UNIX_STREAM, stream_info_->socket_file, 0, blacklog);
    if (!stream_info_->socket) {
        return OSW_ERR;
    }
    return OSW_OK;
}

int ProcessPool::listen(const char *host, int port, int blacklog) {
    if (ipc_mode != OSW_IPC_SOCKET) {
        openswoole_warning("ipc_mode is not OSW_IPC_SOCKET");
        return OSW_ERR;
    }
    stream_info_->socket_file = osw_strdup(host);
    if (stream_info_->socket_file == nullptr) {
        return OSW_ERR;
    }
    stream_info_->socket_port = port;
    stream_info_->socket = make_server_socket(OSW_SOCK_TCP, host, port, blacklog);
    if (!stream_info_->socket) {
        return OSW_ERR;
    }
    return OSW_OK;
}

/**
 * start workers
 */
int ProcessPool::start() {
    if (ipc_mode == OSW_IPC_SOCKET && (stream_info_ == nullptr || stream_info_->socket == 0)) {
        openswoole_warning("must first listen to an tcp port");
        return OSW_ERR;
    }

    uint32_t i;
    running = started = true;
    master_pid = getpid();

    for (i = 0; i < worker_num; i++) {
        workers[i].pool = this;
        workers[i].id = start_id + i;
        workers[i].type = type;
    }

    for (i = 0; i < worker_num; i++) {
        if (spawn(&(workers[i])) < 0) {
            return OSW_ERR;
        }
    }

    return OSW_OK;
}

int ProcessPool::schedule() {
    // schedule by system message queue
    if (schedule_by_sysvmsg) {
        return 0;
    }

    uint32_t i, target_worker_id = 0;
    uint8_t found = 0;

    for (i = 0; i < worker_num + 1; i++) {
        target_worker_id = osw_atomic_fetch_add(&round_id, 1) % worker_num;
        if (workers[target_worker_id].status == OSW_WORKER_IDLE) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        scheduler_warning = 1;
    }
    return target_worker_id;
}

int ProcessPool::response(const char *data, int length) {
    if (stream_info_ == nullptr || stream_info_->last_connection == nullptr ||
        stream_info_->response_buffer == nullptr) {
        openswoole_set_last_error(OSW_ERROR_INVALID_PARAMS);
        return OSW_ERR;
    }
    return stream_info_->response_buffer->append(data, length);
}

/**
 * dispatch data to worker
 */
int ProcessPool::dispatch(EventData *data, int *dst_worker_id) {
    int ret = 0;
    Worker *worker;

    if (use_socket) {
        Stream *stream = Stream::create(stream_info_->socket_file, 0, OSW_SOCK_UNIX_STREAM);
        if (!stream) {
            return OSW_ERR;
        }
        stream->response = nullptr;
        if (stream->send((char *) data, sizeof(data->info) + data->info.len) < 0) {
            stream->cancel = 1;
            delete stream;
            return OSW_ERR;
        }
        return OSW_OK;
    }

    if (*dst_worker_id < 0) {
        *dst_worker_id = schedule();
    }

    *dst_worker_id += start_id;
    worker = get_worker(*dst_worker_id);

    int sendn = sizeof(data->info) + data->info.len;
    ret = worker->send_pipe_message(data, sendn, OSW_PIPE_MASTER | OSW_PIPE_NONBLOCK);

    if (ret >= 0) {
        osw_atomic_fetch_add(&worker->tasking_num, 1);
    } else {
        openswoole_warning("send %d bytes to worker#%d failed", sendn, *dst_worker_id);
    }

    return ret;
}

int ProcessPool::dispatch_blocking(const char *data, uint32_t len) {
    assert(use_socket);

    network::Client _socket(stream_info_->socket->socket_type, false);
    if (!_socket.socket) {
        return OSW_ERR;
    }
    if (_socket.connect(&_socket, stream_info_->socket_file, stream_info_->socket_port, -1, 0) < 0) {
        return OSW_ERR;
    }
    uint32_t packed_len = htonl(len);
    if (_socket.send(&_socket, (char *) &packed_len, 4, 0) < 0) {
        return OSW_ERR;
    }
    if (_socket.send(&_socket, (char *) data, len, 0) < 0) {
        return OSW_ERR;
    }
    _socket.close();
    return OSW_OK;
}

/**
 * dispatch data to worker
 * @return OSW_OK/OSW_ERR
 */
int ProcessPool::dispatch_blocking(EventData *data, int *dst_worker_id) {
    int ret = 0;
    int sendn = sizeof(data->info) + data->info.len;

    if (use_socket) {
        return dispatch_blocking((char *) data, sendn);
    }

    if (*dst_worker_id < 0) {
        *dst_worker_id = schedule();
    }

    *dst_worker_id += start_id;
    Worker *worker = get_worker(*dst_worker_id);

    ret = worker->send_pipe_message(data, sendn, OSW_PIPE_MASTER);
    if (ret < 0) {
        openswoole_warning("send %d bytes to worker#%d failed", sendn, *dst_worker_id);
    } else {
        osw_atomic_fetch_add(&worker->tasking_num, 1);
    }

    return ret > 0 ? OSW_OK : OSW_ERR;
}

void ProcessPool::shutdown() {
    uint32_t i;
    int status;
    Worker *worker;
    running = 0;

    // concurrent kill
    for (i = 0; i < worker_num; i++) {
        worker = &workers[i];
        if (openswoole_kill(worker->pid, SIGTERM) < 0) {
            openswoole_sys_warning("swKill(%d) failed", worker->pid);
            continue;
        }
    }
    for (i = 0; i < worker_num; i++) {
        worker = &workers[i];
        if (openswoole_waitpid(worker->pid, &status, 0) < 0) {
            openswoole_sys_warning("waitpid(%d) failed", worker->pid);
        }
    }
    started = false;
}

pid_t ProcessPool::spawn(Worker *worker) {
    pid_t pid = openswoole_fork(0);
    int ret_code = 0;

    switch (pid) {
    // child
    case 0:
        /**
         * Process start
         */
        if (onWorkerStart != nullptr) {
            onWorkerStart(this, worker->id);
        }
        /**
         * Process main loop
         */
        if (main_loop) {
            ret_code = main_loop(this, worker);
        }
        /**
         * Process stop
         */
        if (onWorkerStop != nullptr) {
            onWorkerStop(this, worker->id);
        }
        exit(ret_code);
        break;
    case -1:
        openswoole_sys_warning("fork() failed");
        break;
        // parent
    default:
        // remove old process
        if (worker->pid) {
            map_->erase(worker->pid);
        }
        worker->pid = pid;
        // insert new process
        map_->emplace(std::make_pair(pid, worker));
        break;
    }
    return pid;
}

int ProcessPool::get_max_request() {
    int task_n;
    if (max_request < 1) {
        return -1;
    } else {
        task_n = max_request;
        if (max_request_grace > 0) {
            task_n += openswoole_system_random(1, max_request_grace);
        }
    }
    return task_n;
}

void ProcessPool::set_max_request(uint32_t _max_request, uint32_t _max_request_grace) {
    max_request = _max_request;
    max_request_grace = _max_request_grace;
}

static int ProcessPool_worker_loop(ProcessPool *pool, Worker *worker) {
    struct {
        long mtype;
        EventData buf;
    } out{};

    ssize_t n = 0, ret, worker_task_always = 0;
    int task_n = pool->get_max_request();
    if (task_n <= 0) {
        worker_task_always = 1;
        task_n = 1;
    }

    /**
     * Use from_fd save the task_worker->id
     */
    out.buf.info.server_fd = worker->id;

    if (pool->schedule_by_sysvmsg) {
        out.mtype = 0;
    } else {
        out.mtype = worker->id + 1;
    }

    while (pool->running && !OpenSwooleWG.shutdown && task_n > 0) {
        /**
         * fetch task
         */
        if (pool->use_msgqueue) {
            n = pool->queue->pop((QueueNode *) &out, sizeof(out.buf));
            if (n < 0 && errno != EINTR) {
                openswoole_sys_warning("[Worker#%d] msgrcv() failed", worker->id);
                break;
            }
        } else if (pool->use_socket) {
            Socket *conn = pool->stream_info_->socket->accept();
            if (conn == nullptr) {
                if (errno == EAGAIN || errno == EINTR) {
                    continue;
                } else {
                    openswoole_sys_warning("accept(%d) failed", pool->stream_info_->socket->get_fd());
                    break;
                }
            }
            n = Stream::recv_blocking(conn, (void *) &out.buf, sizeof(out.buf));
            if (n < 0) {
                conn->free();
                continue;
            }
            pool->stream_info_->last_connection = conn;
        } else {
            n = worker->pipe_worker->read(&out.buf, sizeof(out.buf));
            if (n < 0 && errno != EINTR) {
                openswoole_sys_warning("[Worker#%d] read(%d) failed", worker->id, worker->pipe_worker->fd);
            }
        }

        /**
         * timer
         */
        if (n < 0) {
            if (errno == EINTR && OpenSwooleG.signal_alarm && OpenSwooleTG.timer) {
            _alarm_handler:
                OpenSwooleG.signal_alarm = false;
                OpenSwooleTG.timer->select();
            }
            continue;
        }

        if (n != (ssize_t) (out.buf.info.len + sizeof(out.buf.info))) {
            openswoole_warning("bad task packet, The received data-length[%ld] is inconsistent with the packet-length[%ld]",
                           n,
                           out.buf.info.len + sizeof(out.buf.info));
            continue;
        }

        /**
         * do task
         */
        worker->status = OSW_WORKER_BUSY;
        ret = pool->onTask(pool, &out.buf);
        worker->status = OSW_WORKER_IDLE;

        if (pool->use_socket && pool->stream_info_->last_connection) {
            int _end = 0;
            pool->stream_info_->last_connection->send_blocking((void *) &_end, sizeof(_end));
            pool->stream_info_->last_connection->free();
            pool->stream_info_->last_connection = nullptr;
        }

        /**
         * timer
         */
        if (OpenSwooleG.signal_alarm) {
            goto _alarm_handler;
        }

        if (ret >= 0 && !worker_task_always) {
            task_n--;
        }
    }
    return OSW_OK;
}

void ProcessPool::set_protocol(int task_protocol, uint32_t max_packet_size) {
    if (task_protocol) {
        main_loop = ProcessPool_worker_loop;
    } else {
        packet_buffer = new char[max_packet_size];
        if (stream_info_) {
            stream_info_->response_buffer = new String(OSW_BUFFER_SIZE_STD);
        }
        max_packet_size_ = max_packet_size;
        main_loop = ProcessPool_worker_loop_ex;
    }
}

static int ProcessPool_worker_loop_ex(ProcessPool *pool, Worker *worker) {
    ssize_t n;
    char *data;

    QueueNode *outbuf = (QueueNode *) pool->packet_buffer;
    outbuf->mtype = 0;

    while (pool->running) {
        /**
         * fetch task
         */
        if (pool->use_msgqueue) {
            n = pool->queue->pop(outbuf, OSW_MSGMAX);
            if (n < 0 && errno != EINTR) {
                openswoole_sys_warning("[Worker#%d] msgrcv() failed", worker->id);
                break;
            }
            data = outbuf->mdata;
            outbuf->mtype = 0;
        } else if (pool->use_socket) {
            Socket *conn = pool->stream_info_->socket->accept();
            if (conn == nullptr) {
                if (errno == EAGAIN || errno == EINTR) {
                    continue;
                } else {
                    openswoole_sys_warning("accept(%d) failed", pool->stream_info_->socket->get_fd());
                    break;
                }
            }
            int tmp = 0;
            if (conn->recv_blocking(&tmp, sizeof(tmp), MSG_WAITALL) <= 0) {
                goto _close;
            }
            n = ntohl(tmp);
            if (n <= 0) {
                goto _close;
            } else if (n > pool->max_packet_size_) {
                goto _close;
            }
            if (conn->recv_blocking(pool->packet_buffer, n, MSG_WAITALL) <= 0) {
            _close:
                conn->free();
                continue;
            }
            data = pool->packet_buffer;
            pool->stream_info_->last_connection = conn;
        } else {
            n = worker->pipe_worker->read(pool->packet_buffer, pool->max_packet_size_);
            if (n < 0 && errno != EINTR) {
                openswoole_sys_warning("[Worker#%d] read(%d) failed", worker->id, worker->pipe_worker->fd);
            }
            data = pool->packet_buffer;
        }

        /**
         * timer
         */
        if (n < 0) {
            if (errno == EINTR && OpenSwooleG.signal_alarm && OpenSwooleTG.timer) {
            _alarm_handler:
                OpenSwooleG.signal_alarm = false;
                OpenSwooleTG.timer->select();
            }
            continue;
        }

        pool->onMessage(pool, data, n);

        if (pool->use_socket && pool->stream_info_->last_connection) {
            String *resp_buf = pool->stream_info_->response_buffer;
            if (resp_buf && resp_buf->length > 0) {
                int _l = htonl(resp_buf->length);
                pool->stream_info_->last_connection->send_blocking(&_l, sizeof(_l));
                pool->stream_info_->last_connection->send_blocking(resp_buf->str, resp_buf->length);
                resp_buf->clear();
            }
            pool->stream_info_->last_connection->free();
            pool->stream_info_->last_connection = nullptr;
        }

        /**
         * timer
         */
        if (OpenSwooleG.signal_alarm) {
            goto _alarm_handler;
        }
    }
    return OSW_OK;
}

/**
 * add a worker to pool
 */
int ProcessPool_add_worker(ProcessPool *pool, Worker *worker) {
    pool->map_->emplace(std::make_pair(worker->pid, worker));
    return OSW_OK;
}

bool ProcessPool::detach() {
    if (!running) {
        return false;
    }

    WorkerStopMessage msg;
    msg.pid = getpid();
    msg.worker_id = OpenSwooleG.process_id;

    if (message_box && message_box->push(&msg, sizeof(msg)) < 0) {
        return false;
    }
    if (openswoole_kill(master_pid, SIGIO) < 0) {
        return false;
    }
    running = false;

    return true;
}

int ProcessPool::wait() {
    pid_t new_pid, reload_worker_pid = 0;
    int ret;

    reload_workers = new Worker[worker_num]();
    ON_SCOPE_EXIT {
        delete[] reload_workers;
        reload_workers = nullptr;
    };

    while (running) {
        ExitStatus exit_status = wait_process();

        if (OpenSwooleG.signal_alarm && OpenSwooleTG.timer) {
            OpenSwooleG.signal_alarm = false;
            OpenSwooleTG.timer->select();
        }
        if (read_message) {
            WorkerStopMessage msg;
            while (message_box->pop(&msg, sizeof(msg)) > 0) {
                if (!running) {
                    continue;
                }
                Worker *exit_worker = get_worker_by_pid(msg.pid);
                if (exit_worker == nullptr) {
                    continue;
                }
                pid_t new_pid = spawn(exit_worker);
                if (new_pid < 0) {
                    openswoole_sys_warning("fork worker process failed");
                    return OSW_ERR;
                }
                map_->erase(msg.pid);
            }
            read_message = false;
        }
        if (exit_status.get_pid() < 0) {
            if (!running) {
                break;
            }
            if (!reloading) {
                if (errno > 0 && errno != EINTR) {
                    openswoole_sys_warning("[Manager] wait failed");
                }
                continue;
            } else {
                if (!reload_init) {
                    openswoole_info("reload workers");
                    reload_init = true;
                    memcpy(reload_workers, workers, sizeof(Worker) * worker_num);
                    if (max_wait_time) {
                        openswoole_timer_add((long) (max_wait_time * 1000), false, kill_timeout_worker, this);
                    }
                }
                goto _kill_worker;
            }
        }

        if (running) {
            Worker *exit_worker = get_worker_by_pid(exit_status.get_pid());
            if (exit_worker == nullptr) {
                if (onWorkerNotFound) {
                    onWorkerNotFound(this, exit_status);
                } else {
                    openswoole_warning("[Manager]unknown worker[pid=%d]", exit_status.get_pid());
                }
                continue;
            }

            if (!exit_status.is_normal_exit()) {
                openswoole_warning("worker#%d abnormal exit, status=%d, signal=%d"
                               "%s",
                               exit_worker->id,
                               exit_status.get_code(),
                               exit_status.get_signal(),
                               exit_status.get_signal() == SIGSEGV ? OpenSwooleG.bug_report_message.c_str() : "");
            }
            new_pid = spawn(exit_worker);
            if (new_pid < 0) {
                openswoole_sys_warning("Fork worker process failed");
                return OSW_ERR;
            }
            map_->erase(exit_status.get_pid());
            if (exit_status.get_pid() == reload_worker_pid) {
                reload_worker_i++;
            }
        }
    // reload worker
    _kill_worker:
        if (reloading) {
            // reload finish
            if (reload_worker_i >= worker_num) {
                reloading = reload_init = false;
                reload_worker_pid = reload_worker_i = 0;
                continue;
            }
            reload_worker_pid = reload_workers[reload_worker_i].pid;
            ret = openswoole_kill(reload_worker_pid, SIGTERM);
            if (ret < 0) {
                if (errno == ECHILD) {
                    reload_worker_i++;
                    goto _kill_worker;
                }
                openswoole_sys_warning("[Manager]swKill(%d) failed", reload_workers[reload_worker_i].pid);
                continue;
            }
        }
    }
    return OSW_OK;
}

void ProcessPool::destroy() {
    if (pipes) {
        delete pipes;
        pipes = nullptr;
    }

    if (queue) {
        delete queue;
        queue = nullptr;
    }

    if (stream_info_) {
        if (stream_info_->socket) {
            unlink(stream_info_->socket_file);
            osw_free((void *) stream_info_->socket_file);
        }
        if (stream_info_->socket) {
            stream_info_->socket->free();
            stream_info_->socket = nullptr;
        }
        if (stream_info_->response_buffer) {
            delete stream_info_->response_buffer;
        }
        delete stream_info_;
    }

    if (packet_buffer) {
        delete[] packet_buffer;
    }

    if (map_) {
        delete map_;
    }

    if (message_box) {
        message_box->destroy();
    }

    osw_mem_pool()->free(workers);
}

}  // namespace openswoole
