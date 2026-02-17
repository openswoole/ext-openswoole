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
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  |         Twosee  <twose@qq.com>                                       |
  +----------------------------------------------------------------------+
*/

#pragma once

#include "swoole.h"

#ifdef SW_USE_THREAD_CONTEXT
#include <thread>
#include <mutex>
#else
/* Include fiber headers only when building as PHP extension */
#ifdef ENABLE_PHP_SWOOLE
extern "C" {
#include "main/php.h"
#include "zend_fibers.h"
}
#endif
/* Also include the native context backend (ASM or ucontext) */
#if !defined(SW_USE_ASM_CONTEXT)
#define USE_UCONTEXT 1
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#include <ucontext.h>
#else
#define USE_ASM_CONTEXT 1
#include "swoole_asm_context.h"
#endif
#endif

#if defined(HAVE_VALGRIND) && !defined(HAVE_KQUEUE)
#define USE_VALGRIND 1
#include <valgrind/valgrind.h>
#endif

#ifndef SW_USE_THREAD_CONTEXT
#ifdef USE_UCONTEXT
typedef ucontext_t coroutine_context_t;
#elif defined(USE_ASM_CONTEXT)
typedef fcontext_t coroutine_context_t;
#endif
#endif

typedef std::function<void(void *)> CoroutineFunc;

namespace swoole {
namespace coroutine {

class Context {
  public:
    Context(size_t stack_size, const CoroutineFunc &fn, void *private_data);
    ~Context();
    bool swap_in();
    bool swap_out();
#if !defined(SW_USE_THREAD_CONTEXT) && defined(SW_CONTEXT_DETECT_STACK_USAGE)
    ssize_t get_stack_usage();
#endif

#if !defined(SW_USE_THREAD_CONTEXT)
    char* get_stack() {
#ifdef ENABLE_PHP_SWOOLE
        return native_.stack;
#else
        return stack;
#endif
    }

    size_t get_stack_size() {
        return stack_size_;
    }
#endif

    inline bool is_end() {
        return end_;
    }

  protected:
    CoroutineFunc fn_;
#ifdef SW_USE_THREAD_CONTEXT
    std::thread thread_;
    std::mutex lock_;
    std::mutex *swap_lock_;
#else
#ifdef ENABLE_PHP_SWOOLE
    /* Union for fiber vs native (ASM/ucontext) backends.
     * At runtime, Coroutine::use_fiber_context selects which branch to use. */
    union {
        struct {
            zend_fiber_context fiber_context;
            zend_fiber_context *caller_context;
        } fiber_;
        struct {
            coroutine_context_t ctx;
            coroutine_context_t swap_ctx;
            char *stack;
        } native_;
    };
#else
    /* Core-tests build: only native backend available */
    coroutine_context_t ctx;
    coroutine_context_t swap_ctx;
    char *stack;
#endif
    uint32_t stack_size_;

#ifdef ENABLE_PHP_SWOOLE
    /* Fiber backend helpers (fiber_context.cc) */
    static void fiber_func(zend_fiber_transfer *transfer);
    void fiber_init(size_t stack_size);
    void fiber_destroy();
    bool fiber_swap_in();
    bool fiber_swap_out();
#endif

    /* Native backend helpers (context.cc) */
#if USE_BOOST_V2
    static void context_func_v2(transfer_t transfer);
#else
    static void context_func(void *arg);
#endif
    void native_init(size_t stack_size);
    void native_destroy();
    bool native_swap_in();
    bool native_swap_out();
#endif
#ifdef USE_VALGRIND
    uint32_t valgrind_stack_id;
#endif
    void *private_data_;
    bool end_;
};

}  // namespace coroutine
}  // namespace swoole
