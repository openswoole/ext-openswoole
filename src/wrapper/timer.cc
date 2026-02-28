/**
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
#include "openswoole_timer.h"

using namespace openswoole;

#ifdef __MACH__
Timer *osw_timer() {
    return OpenSwooleTG.timer;
}
#endif

bool openswoole_timer_is_available() {
    return OpenSwooleTG.timer != nullptr;
}

TimerNode *openswoole_timer_add(long ms, bool persistent, const TimerCallback &callback, void *private_data) {
    if (osw_unlikely(!openswoole_timer_is_available())) {
        OpenSwooleTG.timer = new Timer();
        if (osw_unlikely(!OpenSwooleTG.timer->init())) {
            delete OpenSwooleTG.timer;
            OpenSwooleTG.timer = nullptr;
            return nullptr;
        }
    }
    return OpenSwooleTG.timer->add(ms, persistent, private_data, callback);
}

bool openswoole_timer_del(TimerNode *tnode) {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return false;
    }
    return OpenSwooleTG.timer->remove(tnode);
}

void openswoole_timer_delay(TimerNode *tnode, long delay_ms) {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return;
    }
    return OpenSwooleTG.timer->delay(tnode, delay_ms);
}

long openswoole_timer_after(long ms, const TimerCallback &callback, void *private_data) {
    if (ms <= 0) {
        openswoole_warning("Timer must be greater than 0");
        return OSW_ERR;
    }
    TimerNode *tnode = openswoole_timer_add(ms, false, callback, private_data);
    if (tnode == nullptr) {
        return OSW_ERR;
    } else {
        return tnode->id;
    }
}

long openswoole_timer_tick(long ms, const TimerCallback &callback, void *private_data) {
    if (ms <= 0) {
        openswoole_warning("Timer must be greater than 0");
        return OSW_ERR;
    }
    TimerNode *tnode = openswoole_timer_add(ms, true, callback, private_data);
    if (tnode == nullptr) {
        return OSW_ERR;
    } else {
        return tnode->id;
    }
}

bool openswoole_timer_exists(long timer_id) {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return false;
    }
    TimerNode *tnode = OpenSwooleTG.timer->get(timer_id);
    return (tnode && !tnode->removed);
}

bool openswoole_timer_clear(long timer_id) {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return false;
    }
    return OpenSwooleTG.timer->remove(OpenSwooleTG.timer->get(timer_id));
}

TimerNode *openswoole_timer_get(long timer_id) {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return nullptr;
    }
    return OpenSwooleTG.timer->get(timer_id);
}

void openswoole_timer_free() {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return;
    }
    delete OpenSwooleTG.timer;
    OpenSwooleTG.timer = nullptr;
    OpenSwooleG.signal_alarm = false;
}

int openswoole_timer_select() {
    if (!openswoole_timer_is_available()) {
        openswoole_warning("timer is not available");
        return OSW_ERR;
    }
    return OpenSwooleTG.timer->select();
}
