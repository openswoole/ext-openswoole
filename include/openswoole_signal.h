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

#pragma once

#include "openswoole.h"

#include <signal.h>

namespace openswoole {
typedef void (*SignalHandler)(int);

struct Signal {
    SignalHandler handler;
    uint16_t signo;
    bool activated;
};
}  // namespace openswoole

typedef openswoole::SignalHandler oswSignalHandler;

#ifdef HAVE_SIGNALFD
void openswoole_signalfd_init();
bool openswoole_signalfd_setup(openswoole::Reactor *reactor);
#endif

OSW_API oswSignalHandler openswoole_signal_set(int signo, oswSignalHandler func);
OSW_API oswSignalHandler openswoole_signal_set(int signo, oswSignalHandler func, int restart, int mask);
OSW_API oswSignalHandler openswoole_signal_get_handler(int signo);

OSW_API void openswoole_signal_clear(void);
OSW_API void openswoole_signal_block_all(void);
OSW_API char *openswoole_signal_to_str(int sig);
OSW_API void openswoole_signal_callback(int signo);
