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
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
*/

#pragma once

#include "swoole.h"
#include "swoole_coroutine.h"
#include "swoole_coroutine_channel.h"

#include <sys/stat.h>

#include <iostream>
#include <string>
#include <list>
#include <queue>

namespace swoole {
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

    inline void remove_waiting(Coroutine *co,
                               std::vector<Channel *> pull_chans = std::vector<Channel *>(),
                               std::vector<Channel *> push_chans = std::vector<Channel *>()) {
        for (auto &chan : pull_chans) {
            chan->consumer_queue.remove(co);
        }
        for (auto &chan : push_chans) {
            chan->producer_queue.remove(co);
        }
    }

    inline bool all_waiting(std::vector<Channel *> pull_chans = std::vector<Channel *>(),
                            std::vector<Channel *> push_chans = std::vector<Channel *>()) {
        for (auto &chan : pull_chans) {
            if (!chan->is_empty() && chan->consumer_num() == 0) {
                return false;
            }
        }
        for (auto &chan : push_chans) {
            if (!chan->is_empty() && chan->producer_num() == 0) {
                return false;
            }
        }
        return true;
    }
};
//-------------------------------------------------------------------------------
}  // namespace coroutine
}  // namespace swoole
