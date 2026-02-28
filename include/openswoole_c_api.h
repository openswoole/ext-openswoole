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

#ifndef OSW_C_API_H_
#define OSW_C_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum swGlobalHookType {
    OSW_GLOBAL_HOOK_BEFORE_SERVER_START,
    OSW_GLOBAL_HOOK_BEFORE_CLIENT_START,
    OSW_GLOBAL_HOOK_BEFORE_WORKER_START,
    OSW_GLOBAL_HOOK_ON_CORO_START,
    OSW_GLOBAL_HOOK_ON_CORO_STOP,
    OSW_GLOBAL_HOOK_ON_REACTOR_CREATE,
    OSW_GLOBAL_HOOK_BEFORE_SERVER_SHUTDOWN,
    OSW_GLOBAL_HOOK_AFTER_SERVER_SHUTDOWN,
    OSW_GLOBAL_HOOK_BEFORE_WORKER_STOP,
    OSW_GLOBAL_HOOK_ON_REACTOR_DESTROY,
    OSW_GLOBAL_HOOK_BEFORE_SERVER_CREATE,
    OSW_GLOBAL_HOOK_AFTER_SERVER_CREATE,
    OSW_GLOBAL_HOOK_END = OSW_MAX_HOOK_TYPE - 1,
};

typedef void (*swHookFunc)(void *data);

int openswoole_add_function(const char *name, void *func);
void *openswoole_get_function(const char *name, uint32_t length);

int openswoole_add_hook(enum swGlobalHookType type, swHookFunc cb, int push_back);
void openswoole_call_hook(enum swGlobalHookType type, void *arg);
bool openswoole_isset_hook(enum swGlobalHookType type);

const char *openswoole_version(void);
int openswoole_version_id(void);
int openswoole_api_version_id(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif
