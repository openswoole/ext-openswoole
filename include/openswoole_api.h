/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012-2018 The Swoole Group                             |
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
#include "openswoole_coroutine_c_api.h"

enum swEventInitFlag {
    OSW_EVENTLOOP_WAIT_EXIT = 1,
};

OSW_API long openswoole_timer_after(long ms, const openswoole::TimerCallback &callback, void *private_data = nullptr);
OSW_API long openswoole_timer_tick(long ms, const openswoole::TimerCallback &callback, void *private_data = nullptr);
OSW_API openswoole::TimerNode *openswoole_timer_add(long ms,
                                           bool persistent,
                                           const openswoole::TimerCallback &callback,
                                           void *private_data = nullptr);
OSW_API bool openswoole_timer_del(openswoole::TimerNode *tnode);
OSW_API bool openswoole_timer_exists(long timer_id);
OSW_API void openswoole_timer_delay(openswoole::TimerNode *tnode, long delay_ms);
OSW_API openswoole::TimerNode *openswoole_timer_get(long timer_id);
OSW_API bool openswoole_timer_clear(long timer_id);
OSW_API void openswoole_timer_free();
OSW_API int openswoole_timer_select();
OSW_API bool openswoole_timer_is_available();

OSW_API int openswoole_event_init(int flags);
OSW_API int openswoole_event_add(openswoole::network::Socket *socket, int events);
OSW_API int openswoole_event_set(openswoole::network::Socket *socket, int events);
OSW_API int openswoole_event_add_or_update(openswoole::network::Socket *socket, int event);
OSW_API int openswoole_event_del(openswoole::network::Socket *socket);
OSW_API void openswoole_event_defer(openswoole::Callback cb, void *private_data);
OSW_API ssize_t openswoole_event_write(openswoole::network::Socket *socket, const void *data, size_t len);
OSW_API ssize_t openswoole_event_writev(openswoole::network::Socket *socket, const iovec *iov, size_t iovcnt);
OSW_API int openswoole_event_wait();
OSW_API int openswoole_event_free();
OSW_API bool openswoole_event_set_handler(int fdtype, openswoole::ReactorHandler handler);
OSW_API bool openswoole_event_isset_handler(int fdtype);
OSW_API bool openswoole_event_is_available();

#ifdef __MACH__
openswoole::Reactor *osw_reactor();
openswoole::Timer *osw_timer();
#else
#define osw_reactor() (OpenSwooleTG.reactor)
#define osw_timer() (OpenSwooleTG.timer)
#endif
