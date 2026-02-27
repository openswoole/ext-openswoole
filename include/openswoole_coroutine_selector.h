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
*/

#pragma once

#include "openswoole.h"
#include "openswoole_coroutine.h"
#include "openswoole_coroutine_channel.h"

#include <sys/stat.h>

#include <iostream>
#include <string>
#include <list>
#include <queue>

namespace openswoole {
namespace coroutine {
//-------------------------------------------------------------------------------
class Selector {
  public:
    enum ErrorCode {
        ERROR_OK = 0,
        ERROR_TIMEOUT = -1,
        ERROR_CLOSED = -2,
        ERROR_CANCELED = -3,
    };

    struct TimeoutMessage {
        Channel *chan;
        Coroutine *co;
        bool error;
        TimerNode *timer;
    };

    std::pair<std::vector<int>, std::vector<int>> select(std::vector<Channel *> pull_chans = std::vector<Channel *>(),
                                                         std::vector<Channel *> push_chans = std::vector<Channel *>(),
                                                         double timeout = -1);

    Selector() {}

    ~Selector() {}

  protected:
    static void timer_callback(Timer *timer, TimerNode *tnode);

    void remove_waiting(Coroutine *co,
                        std::vector<Channel *> pull_chans = std::vector<Channel *>(),
                        std::vector<Channel *> push_chans = std::vector<Channel *>()) {
        for (auto &chan : pull_chans) {
            chan->consumer_queue.remove(co);
        }
        for (auto &chan : push_chans) {
            chan->producer_queue.remove(co);
        }
    }

    bool is_ready(Channel *chan) {
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
        return ready;
    }

    bool all_waiting(std::vector<Channel *> pull_chans = std::vector<Channel *>(),
                     std::vector<Channel *> push_chans = std::vector<Channel *>()) {
        for (auto &chan : pull_chans) {
            if (is_ready(chan)) {
                return false;
            }
        }
        for (auto &chan : push_chans) {
            if (is_ready(chan)) {
                return false;
            }
        }
        return true;
    }
};
//-------------------------------------------------------------------------------
}  // namespace coroutine
}  // namespace openswoole
