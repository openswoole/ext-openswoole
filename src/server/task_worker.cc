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

namespace openswoole {
using network::Socket;

static void TaskWorker_signal_init(ProcessPool *pool);
static int TaskWorker_onPipeReceive(Reactor *reactor, Event *event);
static int TaskWorker_loop_async(ProcessPool *pool, Worker *worker);
static void TaskWorker_onStart(ProcessPool *pool, int worker_id);
static void TaskWorker_onStop(ProcessPool *pool, int worker_id);
static int TaskWorker_onTask(ProcessPool *pool, EventData *task);

/**
 * after pool->create, before pool->start
 */
void Server::init_task_workers() {
    ProcessPool *pool = &gs->task_workers;
    pool->ptr = this;
    pool->onTask = TaskWorker_onTask;
    pool->onWorkerStart = TaskWorker_onStart;
    pool->onWorkerStop = TaskWorker_onStop;
    /**
     * Make the task worker support asynchronous
     */
    if (task_enable_coroutine) {
        if (task_ipc_mode == TASK_IPC_MSGQUEUE || task_ipc_mode == TASK_IPC_PREEMPTIVE) {
            openswoole_error("cannot use msgqueue when task_enable_coroutine is enable");
            return;
        }
        pool->main_loop = TaskWorker_loop_async;
    }
    if (task_ipc_mode == TASK_IPC_PREEMPTIVE) {
        pool->schedule_by_sysvmsg = true;
    }
}

static int TaskWorker_onTask(ProcessPool *pool, EventData *task) {
    int ret = OSW_OK;
    Server *serv = (Server *) pool->ptr;
    serv->last_task = task;

    if (task->info.type == OSW_SERVER_EVENT_PIPE_MESSAGE) {
        serv->onPipeMessage(serv, task);
    } else {
        ret = serv->onTask(serv, task);
    }

    return ret;
}

bool EventData::pack(const void *_data, size_t _length) {
    if (_length < OSW_IPC_MAX_SIZE - sizeof(info)) {
        memcpy(data, _data, _length);
        info.len = _length;
        return true;
    }

    PacketTask pkg{};
    File file = make_tmpfile();
    if (!file.ready()) {
        return false;
    }

    if (file.write_all(_data, _length) != _length) {
        openswoole_warning("write to tmpfile failed");
        return false;
    }

    info.len = sizeof(pkg);
    OSW_TASK_TYPE(this) |= OSW_TASK_TMPFILE;
    openswoole_strlcpy(pkg.tmpfile, file.get_path().c_str(), sizeof(pkg.tmpfile));
    pkg.length = _length;
    memcpy(data, &pkg, sizeof(pkg));

    return true;
}

bool EventData::unpack(String *buffer) {
    PacketTask _pkg{};
    memcpy(&_pkg, data, sizeof(_pkg));

    File fp(_pkg.tmpfile, O_RDONLY);
    if (!fp.ready()) {
        openswoole_sys_warning("open(%s) failed", _pkg.tmpfile);
        return false;
    }
    if (buffer->size < _pkg.length && !buffer->extend(_pkg.length)) {
        return false;
    }
    if (fp.read_all(buffer->str, _pkg.length) != _pkg.length) {
        return false;
    }
    if (!(OSW_TASK_TYPE(this) & OSW_TASK_PEEK)) {
        unlink(_pkg.tmpfile);
    }
    buffer->length = _pkg.length;
    return true;
}

static void TaskWorker_signal_init(ProcessPool *pool) {
    /**
     * use user settings
     */
    OpenSwooleG.use_signalfd = OpenSwooleG.enable_signalfd;

    openswoole_signal_set(SIGHUP, nullptr);
    openswoole_signal_set(SIGPIPE, nullptr);
    openswoole_signal_set(SIGUSR1, Server::worker_signal_handler);
    openswoole_signal_set(SIGUSR2, nullptr);
    openswoole_signal_set(SIGTERM, Server::worker_signal_handler);
#ifdef SIGRTMIN
    openswoole_signal_set(SIGRTMIN, Server::worker_signal_handler);
#endif
}

static void TaskWorker_onStart(ProcessPool *pool, int worker_id) {
    Server *serv = (Server *) pool->ptr;
    OpenSwooleG.process_id = worker_id;

    /**
     * Make the task worker support asynchronous
     */
    if (serv->task_enable_coroutine) {
        if (openswoole_event_init(0) < 0) {
            openswoole_error("[TaskWorker] create reactor failed");
            return;
        }
        OpenSwooleG.enable_signalfd = 1;
    } else {
        OpenSwooleG.enable_signalfd = 0;
        OpenSwooleTG.reactor = nullptr;
    }

    TaskWorker_signal_init(pool);
    serv->worker_start_callback();

    Worker *worker = pool->get_worker(worker_id);
    worker->start_time = ::time(nullptr);
    worker->request_count = 0;
    OpenSwooleWG.worker = worker;
    OpenSwooleWG.worker->status = OSW_WORKER_IDLE;
    /**
     * task_max_request
     */
    if (pool->max_request > 0) {
        OpenSwooleWG.run_always = false;
        OpenSwooleWG.max_request = pool->get_max_request();
    } else {
        OpenSwooleWG.run_always = true;
    }
}

static void TaskWorker_onStop(ProcessPool *pool, int worker_id) {
    openswoole_event_free();
    Server *serv = (Server *) pool->ptr;
    serv->worker_stop_callback();
}

/**
 * receive data from worker process
 */
static int TaskWorker_onPipeReceive(Reactor *reactor, Event *event) {
    EventData task;
    ProcessPool *pool = (ProcessPool *) reactor->ptr;
    Worker *worker = OpenSwooleWG.worker;
    Server *serv = (Server *) pool->ptr;

    if (event->socket->read(&task, sizeof(task)) > 0) {
        worker->status = OSW_WORKER_BUSY;
        int retval = TaskWorker_onTask(pool, &task);
        worker->status = OSW_WORKER_IDLE;
        worker->request_count++;
        // maximum number of requests, process will exit.
        if (!OpenSwooleWG.run_always && worker->request_count >= OpenSwooleWG.max_request) {
            serv->stop_async_worker(worker);
        }
        return retval;
    } else {
        openswoole_sys_warning("read(%d, %ld) failed", event->fd, sizeof(task));
        return OSW_ERR;
    }
}

/**
 * async task worker
 */
static int TaskWorker_loop_async(ProcessPool *pool, Worker *worker) {
    Server *serv = (Server *) pool->ptr;
    Socket *socket = worker->pipe_worker;
    worker->status = OSW_WORKER_IDLE;

    socket->set_nonblock();
    osw_reactor()->ptr = pool;
    openswoole_event_add(socket, OSW_EVENT_READ);
    openswoole_event_set_handler(OSW_FD_PIPE, TaskWorker_onPipeReceive);

    for (uint i = 0; i < serv->worker_num + serv->task_worker_num; i++) {
        worker = serv->get_worker(i);
        worker->pipe_master->buffer_size = UINT_MAX;
        worker->pipe_worker->buffer_size = UINT_MAX;
    }

    serv->start_event_loop_lag_timer(OpenSwooleWG.worker);

    return openswoole_event_wait();
}

/**
 * Send the task result to worker
 */
int Server::reply_task_result(const char *data, size_t data_len, int flags, EventData *current_task) {
    EventData buf;
    osw_memset_zero(&buf.info, sizeof(buf.info));
    if (task_worker_num < 1) {
        openswoole_warning("cannot use Server::task()/Server::finish() method, because no set [task_worker_num]");
        return OSW_ERR;
    }
    if (current_task == nullptr) {
        current_task = last_task;
    }
    if (current_task->info.type == OSW_SERVER_EVENT_PIPE_MESSAGE) {
        openswoole_warning("Server::task()/Server::finish() is not supported in onPipeMessage callback");
        return OSW_ERR;
    }
    if (OSW_TASK_TYPE(current_task) & OSW_TASK_NOREPLY) {
        openswoole_warning("Server::finish() can only be used in the worker process");
        return OSW_ERR;
    }

    uint16_t source_worker_id = current_task->info.reactor_id;
    Worker *worker = get_worker(source_worker_id);

    if (worker == nullptr) {
        openswoole_warning("invalid worker_id[%d]", source_worker_id);
        return OSW_ERR;
    }

    int ret;
    // for openswoole_server_task
    if (OSW_TASK_TYPE(current_task) & OSW_TASK_NONBLOCK) {
        buf.info.type = OSW_SERVER_EVENT_FINISH;
        buf.info.fd = current_task->info.fd;
        buf.info.time = microtime();
        buf.info.reactor_id = OpenSwooleWG.worker->id;
        // callback function
        if (OSW_TASK_TYPE(current_task) & OSW_TASK_CALLBACK) {
            flags |= OSW_TASK_CALLBACK;
        } else if (OSW_TASK_TYPE(current_task) & OSW_TASK_COROUTINE) {
            flags |= OSW_TASK_COROUTINE;
        }
        OSW_TASK_TYPE(&buf) = flags;

        // write to file
        if (!buf.pack(data, data_len)) {
            openswoole_warning("large task pack failed()");
            return OSW_ERR;
        }

        if (worker->pool->use_socket && worker->pool->stream_info_->last_connection) {
            uint32_t _len = htonl(data_len);
            ret = worker->pool->stream_info_->last_connection->send_blocking((void *) &_len, sizeof(_len));
            if (ret > 0) {
                ret = worker->pool->stream_info_->last_connection->send_blocking(data, data_len);
            }
        } else {
            ret = send_to_worker_from_worker(worker, &buf, sizeof(buf.info) + buf.info.len, OSW_PIPE_MASTER);
        }
    } else {
        uint64_t flag = 1;

        /**
         * Use worker shm store the result
         */
        EventData *result = &(task_result[source_worker_id]);
        Pipe *pipe = task_notify_pipes.at(source_worker_id).get();

        // lock worker
        worker->lock->lock();

        if (OSW_TASK_TYPE(current_task) & OSW_TASK_WAITALL) {
            osw_atomic_t *finish_count = (osw_atomic_t *) result->data;
            char *_tmpfile = result->data + 4;
            File file(_tmpfile, O_APPEND | O_WRONLY);
            if (file.ready()) {
                buf.info.type = OSW_SERVER_EVENT_FINISH;
                buf.info.fd = current_task->info.fd;
                OSW_TASK_TYPE(&buf) = flags;
                if (!buf.pack(data, data_len)) {
                    openswoole_warning("large task pack failed()");
                    buf.info.len = 0;
                }
                size_t bytes = sizeof(buf.info) + buf.info.len;
                if (file.write_all(&buf, bytes) != bytes) {
                    openswoole_sys_warning("write(%s, %ld) failed", _tmpfile, bytes);
                }
                osw_atomic_fetch_add(finish_count, 1);
            }
        } else {
            result->info.type = OSW_SERVER_EVENT_FINISH;
            result->info.fd = current_task->info.fd;
            OSW_TASK_TYPE(result) = flags;
            if (!result->pack(data, data_len)) {
                // unlock worker
                worker->lock->unlock();
                openswoole_warning("large task pack failed()");
                return OSW_ERR;
            }
        }

        // unlock worker
        worker->lock->unlock();

        while (1) {
            ret = pipe->write(&flag, sizeof(flag));
            auto _sock = pipe->get_socket(true);
            if (ret < 0 && _sock->catch_error(errno) == OSW_WAIT) {
                if (_sock->wait_event(-1, OSW_EVENT_WRITE) == 0) {
                    continue;
                }
            }
            break;
        }
    }
    if (ret < 0) {
        if (openswoole_get_last_error() == EAGAIN || openswoole_get_last_error() == OSW_ERROR_SOCKET_POLL_TIMEOUT) {
            openswoole_warning("send result to worker timed out");
        } else {
            openswoole_sys_warning("send result to worker failed");
        }
    }
    return ret;
}
}  // namespace openswoole
