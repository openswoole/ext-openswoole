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
#include "openswoole_reactor.h"
#include "openswoole_timer.h"

#if !defined(HAVE_CLOCK_GETTIME) && defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach_time.h>
#include <sys/sysctl.h>

#define ORWL_NANO (+1.0E-9)
#define ORWL_GIGA UINT64_C(1000000000)

static double orwl_timebase = 0.0;
static uint64_t orwl_timestart = 0;

int openswoole_clock_gettime(int which_clock, struct timespec *t) {
    // be more careful in a multithreaded environement
    if (!orwl_timestart) {
        mach_timebase_info_data_t tb = {0};
        mach_timebase_info(&tb);
        orwl_timebase = tb.numer;
        orwl_timebase /= tb.denom;
        orwl_timestart = mach_absolute_time();
    }
    double diff = (mach_absolute_time() - orwl_timestart) * orwl_timebase;
    t->tv_sec = diff * ORWL_NANO;
    t->tv_nsec = diff - (t->tv_sec * ORWL_GIGA);
    return 0;
}
#endif

namespace openswoole {

Timer::Timer() : heap(1024, Heap::MIN_HEAP) {
    _current_id = -1;
    next_msec_ = -1;
    _next_id = 1;
    round = 0;
    now(&base_time);
}

bool Timer::init() {
    if (now(&base_time) < 0) {
        return false;
    }
    if (OpenSwooleTG.reactor) {
        return init_reactor(OpenSwooleTG.reactor);
    } else {
        return init_system_timer();
    }
}

bool Timer::init_reactor(Reactor *reactor) {
    reactor_ = reactor;
    set = [](Timer *timer, long exec_msec) -> int {
        timer->reactor_->timeout_msec = exec_msec;
        return OSW_OK;
    };
    close = [](Timer *timer) { timer->set(timer, -1); };

    reactor->set_end_callback(Reactor::PRIORITY_TIMER, [this](Reactor *) { select(); });

    reactor->set_exit_condition(Reactor::EXIT_CONDITION_TIMER,
                                [this](Reactor *reactor, size_t &event_num) -> bool { return count() == 0; });

    reactor->add_destroy_callback([](void *) {
        if (openswoole_timer_is_available()) {
            openswoole_timer_free();
        }
    });

    return true;
}

void Timer::reinit(Reactor *reactor) {
    init_reactor(reactor);
    reactor->timeout_msec = next_msec_;
}

Timer::~Timer() {
    if (close) {
        close(this);
    }
    for (auto iter = map.begin(); iter != map.end(); iter++) {
        auto tnode = iter->second;
        delete tnode;
    }
}

TimerNode *Timer::add(long _msec, bool persistent, void *data, const TimerCallback &callback) {
    if (osw_unlikely(_msec <= 0)) {
        openswoole_error_log(OSW_LOG_WARNING, OSW_ERROR_INVALID_PARAMS, "msec value[%ld] is invalid", _msec);
        return nullptr;
    }

    int64_t now_msec = get_relative_msec();
    if (osw_unlikely(now_msec < 0)) {
        return nullptr;
    }

    TimerNode *tnode = new TimerNode();
    tnode->data = data;
    tnode->type = TimerNode::TYPE_KERNEL;
    tnode->exec_msec = now_msec + _msec;
    tnode->interval = persistent ? _msec : 0;
    tnode->removed = false;
    tnode->callback = callback;
    tnode->round = round;
    tnode->destructor = nullptr;

    if (next_msec_ < 0 || next_msec_ > _msec) {
        set(this, _msec);
        next_msec_ = _msec;
    }

    tnode->id = _next_id++;
    if (osw_unlikely(tnode->id < 0)) {
        tnode->id = 1;
        _next_id = 2;
    }

    tnode->heap_node = heap.push(tnode->exec_msec, tnode);
    if (osw_unlikely(tnode->heap_node == nullptr)) {
        delete tnode;
        return nullptr;
    }
    map.emplace(std::make_pair(tnode->id, tnode));
    openswoole_trace_log(OSW_TRACE_TIMER,
                     "id=%ld, exec_msec=%" PRId64 ", msec=%ld, round=%" PRIu64 ", exist=%lu",
                     tnode->id,
                     tnode->exec_msec,
                     _msec,
                     tnode->round,
                     count());
    return tnode;
}

bool Timer::remove(TimerNode *tnode) {
    if (osw_unlikely(!tnode || tnode->removed)) {
        return false;
    }
    if (osw_unlikely(_current_id > 0 && tnode->id == _current_id)) {
        tnode->removed = true;
        openswoole_trace_log(OSW_TRACE_TIMER,
                         "set-remove: id=%ld, exec_msec=%" PRId64 ", round=%" PRIu64 ", exist=%lu",
                         tnode->id,
                         tnode->exec_msec,
                         tnode->round,
                         count());
        return true;
    }
    if (osw_unlikely(!map.erase(tnode->id))) {
        return false;
    }
    if (tnode->heap_node) {
        heap.remove(tnode->heap_node);
    }
    if (tnode->destructor) {
        tnode->destructor(tnode);
    }
    openswoole_trace_log(OSW_TRACE_TIMER,
                     "id=%ld, exec_msec=%" PRId64 ", round=%" PRIu64 ", exist=%lu",
                     tnode->id,
                     tnode->exec_msec,
                     tnode->round,
                     count());
    delete tnode;
    return true;
}

int Timer::select() {
    int64_t now_msec = get_relative_msec();
    if (osw_unlikely(now_msec < 0)) {
        return OSW_ERR;
    }

    TimerNode *tnode = nullptr;
    HeapNode *tmp;

    openswoole_trace_log(OSW_TRACE_TIMER, "timer msec=%" PRId64 ", round=%" PRId64, now_msec, round);

    while ((tmp = heap.top())) {
        tnode = (TimerNode *) tmp->data;
        if (tnode->exec_msec > now_msec || tnode->round == round) {
            break;
        }

        _current_id = tnode->id;
        if (!tnode->removed) {
            openswoole_trace_log(OSW_TRACE_TIMER,
                             "id=%ld, exec_msec=%" PRId64 ", round=%" PRIu64 ", exist=%lu",
                             tnode->id,
                             tnode->exec_msec,
                             tnode->round,
                             count() - 1);
            tnode->callback(this, tnode);
        }
        _current_id = -1;

        // persistent timer
        if (tnode->interval > 0 && !tnode->removed) {
            while (tnode->exec_msec <= now_msec) {
                tnode->exec_msec += tnode->interval;
            }
            heap.change_priority(tnode->exec_msec, tmp);
            continue;
        }

        heap.pop();
        map.erase(tnode->id);
        delete tnode;
    }

    if (!tnode || !tmp) {
        next_msec_ = -1;
        set(this, -1);
    } else {
        next_msec_ = tnode->exec_msec - now_msec;
        if (next_msec_ <= 0) {
            next_msec_ = 1;
        }
        set(this, next_msec_);
    }
    round++;

    return OSW_OK;
}

#if defined(OSW_USE_MONOTONIC_TIME) && defined(CLOCK_MONOTONIC)
int Timer::now(struct timeval *time) {
    struct timespec _now;
    if (clock_gettime(CLOCK_MONOTONIC, &_now) < 0) {
        openswoole_sys_warning("clock_gettime(CLOCK_MONOTONIC) failed");
        return OSW_ERR;
    }
    time->tv_sec = _now.tv_sec;
    time->tv_usec = _now.tv_nsec / 1000;
#else
if (gettimeofday(time, nullptr) < 0) {
    openswoole_sys_warning("gettimeofday() failed");
    return OSW_ERR;
}
#endif
    return OSW_OK;
}  // namespace openswoole

};  // namespace openswoole
