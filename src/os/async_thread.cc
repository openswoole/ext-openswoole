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
#include "openswoole_socket.h"
#include "openswoole_reactor.h"
#include "openswoole_string.h"
#include "openswoole_signal.h"
#include "openswoole_pipe.h"
#include "openswoole_async.h"
#include "openswoole_util.h"

#include <thread>
#include <atomic>
#include <unordered_map>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <sstream>

namespace openswoole {
namespace async {
//-------------------------------------------------------------------------------
class EventQueue {
  public:
    inline void push(AsyncEvent *event) {
        _queue.push(event);
    }

    inline AsyncEvent *pop() {
        if (_queue.empty()) {
            return nullptr;
        }
        AsyncEvent *retval = _queue.front();
        _queue.pop();
        return retval;
    }

    inline double get_max_wait_time() {
        if (_queue.empty()) {
            return 0;
        } else {
            AsyncEvent *event = _queue.front();
            return microtime() - event->timestamp;
        }
    }

    inline size_t count() {
        return _queue.size();
    }

  private:
    std::queue<AsyncEvent *> _queue;
};

class ThreadPool {
  public:
    ThreadPool(size_t _core_worker_num, size_t _worker_num, double _max_wait_time, double _max_idle_time) {
        running = false;

        core_worker_num = _core_worker_num == 0 ? OSW_CPU_NUM : OSW_MAX(1, _core_worker_num);
        worker_num = _worker_num == 0 ? OSW_CPU_NUM * OSW_AIO_THREAD_NUM_MULTIPLE : OSW_MAX(core_worker_num, _worker_num);
        max_wait_time = _max_wait_time == 0 ? OSW_AIO_TASK_MAX_WAIT_TIME : _max_wait_time;
        max_idle_time = _max_idle_time == 0 ? OSW_AIO_THREAD_MAX_IDLE_TIME : _max_idle_time;
    }

    ~ThreadPool() {
        shutdown();
    }

    bool start() {
        running = true;
        current_task_id = 0;
        n_waiting = 0;
        n_closing = 0;
        for (size_t i = 0; i < core_worker_num; i++) {
            create_thread(true);
        }
        return true;
    }

    bool shutdown() {
        if (!running) {
            return false;
        }

        event_mutex.lock();
        running = false;
        _cv.notify_all();
        event_mutex.unlock();

        for (auto &i : threads) {
            std::thread *_thread = i.second;
            if (_thread->joinable()) {
                _thread->join();
            }
            delete _thread;
        }

        return true;
    }

    void schedule() {
        if (n_waiting == 0 && threads.size() < worker_num && max_wait_time > 0) {
            event_mutex.lock();
            double _max_wait_time = _queue.get_max_wait_time();
            event_mutex.unlock();

            if (_max_wait_time > max_wait_time) {
                size_t n = 1;
                /**
                 * maybe we can find a better strategy
                 */
                if (threads.size() + n > worker_num) {
                    n = worker_num - threads.size();
                }
                openswoole_trace_log(OSW_TRACE_AIO,
                                 "Create %zu thread due to wait %fs, we will have %zu threads",
                                 n,
                                 _max_wait_time,
                                 threads.size() + n);
                while (n--) {
                    create_thread();
                }
            }
        }
    }

    AsyncEvent *dispatch(const AsyncEvent *request) {
        if (OpenSwooleTG.async_threads->schedule) {
            schedule();
        }
        auto _event_copy = new AsyncEvent(*request);
        _event_copy->task_id = current_task_id++;
        _event_copy->timestamp = microtime();
        _event_copy->pipe_socket = OpenSwooleTG.async_threads->write_socket;
        event_mutex.lock();
        _queue.push(_event_copy);
        _cv.notify_one();
        event_mutex.unlock();
        openswoole_debug("push and notify one: %f", microtime());
        return _event_copy;
    }

    inline size_t worker_count() {
        return threads.size();
    }

    inline size_t queue_count() {
        std::unique_lock<std::mutex> lock(event_mutex);
        return _queue.count();
    }

    static std::string get_thread_id(std::thread::id id) {
        std::stringstream ss;
        ss << id;
        return ss.str();
    }

    void release_thread(std::thread::id tid) {
        auto i = threads.find(tid);
        if (i == threads.end()) {
            openswoole_warning("AIO thread#%s is missing", get_thread_id(tid).c_str());
            return;
        } else {
            std::thread *_thread = i->second;
            openswoole_trace_log(OSW_TRACE_AIO,
                             "release idle thread#%s, we have %zu now",
                             get_thread_id(tid).c_str(),
                             threads.size() - 1);
            if (_thread->joinable()) {
                _thread->join();
            }
            threads.erase(i);
            delete _thread;
        }
    }

    static void release_callback(AsyncEvent *event) {
        std::thread::id *tid = reinterpret_cast<std::thread::id *>(event->object);
        OpenSwooleTG.async_threads->pool->release_thread(*tid);
        delete tid;
        // balance
        OpenSwooleTG.async_threads->task_num++;
    }

    void notify_one() {
        _cv.notify_one();
    }

  private:
    void create_thread(const bool is_core_worker = false);

    size_t core_worker_num;
    size_t worker_num;
    double max_wait_time;
    double max_idle_time;

    bool running;

    std::atomic<size_t> n_waiting;
    std::atomic<size_t> n_closing;
    size_t current_task_id = 0;
    std::unordered_map<std::thread::id, std::thread *> threads;
    EventQueue _queue;
    std::mutex event_mutex;
    std::condition_variable _cv;
};

void ThreadPool::create_thread(const bool is_core_worker) {
    try {
        std::thread *_thread = new std::thread([this, is_core_worker]() {
            bool exit_flag = false;
            OpenSwooleTG.buffer_stack = new String(OSW_STACK_BUFFER_SIZE);
            ON_SCOPE_EXIT {
                delete OpenSwooleTG.buffer_stack;
                OpenSwooleTG.buffer_stack = nullptr;
            };

            openswoole_signal_block_all();

            while (running) {
                event_mutex.lock();
                AsyncEvent *event = _queue.pop();
                event_mutex.unlock();

                openswoole_debug("%s: %f", event ? "pop 1 event" : "no event", microtime());

                if (event) {
                    if (osw_unlikely(event->handler == nullptr)) {
                        event->error = OSW_ERROR_AIO_BAD_REQUEST;
                        event->retval = -1;
                    } else if (osw_unlikely(event->canceled)) {
                        event->error = OSW_ERROR_AIO_CANCELED;
                        event->retval = -1;
                    } else {
                        event->handler(event);
                    }

                    openswoole_trace_log(OSW_TRACE_AIO,
                                     "aio_thread %s. ret=%ld, error=%d",
                                     event->retval > 0 ? "ok" : "failed",
                                     event->retval,
                                     event->error);

                _send_event:
                    while (true) {
                        ssize_t ret = event->pipe_socket->write(&event, sizeof(event));
                        if (ret < 0) {
                            if (errno == EAGAIN) {
                                event->pipe_socket->wait_event(1000, OSW_EVENT_WRITE);
                                continue;
                            } else if (errno == EINTR) {
                                continue;
                            } else {
                                delete event;
                                openswoole_sys_warning("sendto openswoole_aio_pipe_write failed");
                            }
                        }
                        break;
                    }

                    // exit
                    if (exit_flag) {
                        n_closing--;
                        break;
                    }
                } else {
                    std::unique_lock<std::mutex> lock(event_mutex);
                    if (_queue.count() > 0) {
                        continue;
                    }
                    if (!running) {
                        break;
                    }
                    ++n_waiting;
                    if (is_core_worker || max_idle_time <= 0) {
                        _cv.wait(lock);
                    } else {
                        while (true) {
                            if (_cv.wait_for(lock, std::chrono::microseconds((size_t) (max_idle_time * 1000 * 1000))) ==
                                std::cv_status::timeout) {
                                if (running && n_closing != 0) {
                                    // wait for the next round
                                    continue;
                                }
                                /* notifies the main thread to release this thread */
                                event = new AsyncEvent;
                                event->object = new std::thread::id(std::this_thread::get_id());
                                event->callback = release_callback;
                                event->pipe_socket = OpenSwooleG.aio_default_socket;
                                event->canceled = false;

                                --n_waiting;
                                ++n_closing;
                                exit_flag = true;
                                goto _send_event;
                            }
                            break;
                        }
                    }
                    --n_waiting;
                }
            }
        });
        threads[_thread->get_id()] = _thread;
    } catch (const std::system_error &e) {
        openswoole_sys_notice("create aio thread failed, please check your system configuration or adjust aio_worker_num");
        return;
    }
}

AsyncEvent *dispatch(const AsyncEvent *request) {
    if (osw_unlikely(!OpenSwooleTG.async_threads)) {
        OpenSwooleTG.async_threads = new AsyncThreads();
    }
    AsyncEvent *event = OpenSwooleTG.async_threads->pool->dispatch(request);
    if (osw_likely(event)) {
        OpenSwooleTG.async_threads->task_num++;
    }
    return event;
}

//-------------------------------------------------------------------------------
}  // namespace async

int AsyncThreads::callback(Reactor *reactor, Event *event) {
    if (OpenSwooleTG.async_threads->schedule) {
        OpenSwooleTG.async_threads->pool->schedule();
    }

    AsyncEvent *events[OSW_AIO_EVENT_NUM];
    ssize_t n = event->socket->read(events, sizeof(AsyncEvent *) * OSW_AIO_EVENT_NUM);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return OSW_OK;
        }
        openswoole_sys_warning("read() aio events failed");
        return OSW_ERR;
    }
    for (size_t i = 0; i < n / sizeof(AsyncEvent *); i++) {
        AsyncEvent *event = events[i];
        if (!event->canceled) {
            event->callback(event);
        }
        OpenSwooleTG.async_threads->task_num--;
        delete event;
    }

    return OSW_OK;
}

size_t AsyncThreads::thread_count() {
    return pool ? pool->worker_count() : 0;
}

void AsyncThreads::notify_one() {
    if (pool) {
        pool->notify_one();
    }
}

AsyncThreads::AsyncThreads() {
    if (!OpenSwooleTG.reactor) {
        openswoole_warning("no event loop, cannot initialized");
        throw openswoole::Exception(OSW_ERROR_WRONG_OPERATION);
    }

    pipe = new Pipe(false);
    if (!pipe->ready()) {
        delete pipe;
        pipe = nullptr;
        openswoole_throw_error(OSW_ERROR_SYSTEM_CALL_FAIL);
    }

    read_socket = pipe->get_socket(false);
    write_socket = pipe->get_socket(true);
    read_socket->fd_type = OSW_FD_AIO;
    write_socket->fd_type = OSW_FD_AIO;

    openswoole_event_add(read_socket, OSW_EVENT_READ);

    osw_reactor()->add_destroy_callback([](void *data) {
        if (!OpenSwooleTG.async_threads) {
            return;
        }
        openswoole_event_del(OpenSwooleTG.async_threads->read_socket);
        delete OpenSwooleTG.async_threads;
        OpenSwooleTG.async_threads = nullptr;
    });

    osw_reactor()->set_exit_condition(Reactor::EXIT_CONDITION_AIO_TASK, [](Reactor *reactor, size_t &event_num) -> bool {
        if (OpenSwooleTG.async_threads && OpenSwooleTG.async_threads->task_num == 0) {
            event_num--;
        }
        return true;
    });

    init_lock.lock();
    pool = new async::ThreadPool(
        OpenSwooleG.aio_core_worker_num, OpenSwooleG.aio_worker_num, OpenSwooleG.aio_max_wait_time, OpenSwooleG.aio_max_idle_time);
    pool->start();
    schedule = true;
    init_lock.unlock();

    OpenSwooleG.aio_default_socket = write_socket;
    OpenSwooleTG.async_threads = this;
}

AsyncThreads::~AsyncThreads() {
    delete pool;
    pool = nullptr;
    pipe->close();
    read_socket = nullptr;
    write_socket = nullptr;
    delete pipe;
    pipe = nullptr;
}
};  // namespace openswoole
