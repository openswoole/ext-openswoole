/*
  +----------------------------------------------------------------------+
  | Open Swoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@openswoole.com so we can mail you a copy immediately.          |
  +----------------------------------------------------------------------+
*/

#include "swoole_coroutine_selector.h"

namespace swoole {
namespace coroutine {

void Selector::timer_callback(Timer *timer, TimerNode *tnode) {
    TimeoutMessage *msg = (TimeoutMessage *) tnode->data;
    msg->error = true;
    msg->timer = nullptr;
    msg->co->resume();
}

std::pair<std::vector<int>, std::vector<int>> Selector::select(std::vector<Channel *> pull_chans,
                                                               std::vector<Channel *> push_chans,
                                                               double timeout) {
    std::vector<int> pull_ready;
    std::vector<int> push_ready;

    if (pull_chans.empty() && push_chans.empty()) {
        return std::make_pair(pull_ready, push_ready);
    }

    Coroutine *current_co = Coroutine::get_current_safe();

    if (all_waiting(pull_chans, push_chans)) {
        TimeoutMessage msg;
        msg.error = false;
        msg.timer = nullptr;
        if (timeout > 0) {
            long msec = (long) (timeout * 1000);
            msg.co = current_co;
            msg.timer = swoole_timer_add(msec, false, timer_callback, &msg);
        }

        for (auto &chan : pull_chans) {
            chan->consumer_queue.push_back(current_co);
        }

        for (auto &chan : push_chans) {
            chan->producer_queue.push_back(current_co);
        }

        // yield
        current_co->yield();

        if (msg.timer) {
            swoole_timer_del(msg.timer);
        }
    }

    remove_waiting(current_co, pull_chans, push_chans);

    int index = 0;
    for (auto &chan : pull_chans) {
        bool ready = false;
        if (chan->get_error() > 0) {
            ready = true;
        }

        if (chan->is_closed() && chan->is_empty()) {
            ready = true;
        }

        if (!chan->is_empty()) {
            ready = true;
        }
        if(ready) {
            pull_ready.push_back(index);
        }
        index++;
    }

    index = 0;
    for (auto &chan : push_chans) {
        bool ready = false;
        if (chan->get_error() > 0) {
            ready = true;
        }

        if (chan->is_closed() && chan->is_empty()) {
            ready = true;
        }

        if (!chan->is_empty()) {
            ready = true;
        }
        if(ready) {
            pull_ready.push_back(index);
        }
        index++;
    }

    return std::make_pair(pull_ready, push_ready);
    ;
}

}  // namespace coroutine
}  // namespace swoole
