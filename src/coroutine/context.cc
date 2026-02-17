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
  +----------------------------------------------------------------------+
*/

#include "swoole_coroutine_context.h"

#ifdef SW_CONTEXT_PROTECT_STACK_PAGE
#include <sys/mman.h>
#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
#define MAP_ANONYMOUS MAP_ANON
#endif
#endif

#ifndef SW_USE_THREAD_CONTEXT

#include "swoole_coroutine.h"

#define MAGIC_STRING "swoole_coroutine#5652a7fb2b38be"
#define START_OFFSET (64 * 1024)

/* Member access macros: PHP build uses union (native_.), core-tests use direct members */
#ifdef ENABLE_PHP_SWOOLE
#define CTX_STACK native_.stack
#define CTX_CTX native_.ctx
#define CTX_SWAP_CTX native_.swap_ctx
#else
#define CTX_STACK stack
#define CTX_CTX ctx
#define CTX_SWAP_CTX swap_ctx
#endif

namespace swoole {
namespace coroutine {

/* ---- Dispatch: constructor, destructor, swap_in, swap_out ---- */

Context::Context(size_t stack_size, const CoroutineFunc &fn, void *private_data)
    : fn_(fn), stack_size_(stack_size), private_data_(private_data) {
    end_ = false;
#ifdef ENABLE_PHP_SWOOLE
    if (Coroutine::use_fiber_context) {
        fiber_init(stack_size);
    } else {
        native_init(stack_size);
    }
#else
    native_init(stack_size);
#endif
}

Context::~Context() {
#ifdef ENABLE_PHP_SWOOLE
    if (Coroutine::use_fiber_context) {
        fiber_destroy();
    } else {
        native_destroy();
    }
#else
    native_destroy();
#endif
}

bool Context::swap_in() {
#ifdef ENABLE_PHP_SWOOLE
    if (Coroutine::use_fiber_context) {
        return fiber_swap_in();
    }
#endif
    return native_swap_in();
}

bool Context::swap_out() {
#ifdef ENABLE_PHP_SWOOLE
    if (Coroutine::use_fiber_context) {
        return fiber_swap_out();
    }
#endif
    return native_swap_out();
}

/* ---- Native backend (ASM/ucontext) ---- */

void Context::native_init(size_t stack_size) {
#ifdef SW_CONTEXT_PROTECT_STACK_PAGE
    int mapflags = MAP_PRIVATE | MAP_ANONYMOUS;
#ifdef __OpenBSD__
    // no-op for Linux and NetBSD, not to enable on FreeBSD as the semantic differs.
    // However necessary on OpenBSD.
    mapflags |= MAP_STACK;
#endif
    CTX_STACK = (char *) ::mmap(0, stack_size_, PROT_READ | PROT_WRITE, mapflags, -1, 0);
#else
    CTX_STACK = (char *) sw_malloc(stack_size_);
#endif
    if (!CTX_STACK) {
        swoole_fatal_error(SW_ERROR_MALLOC_FAIL, "failed to malloc stack memory.");
        exit(254);
    }
    swoole_trace_log(SW_TRACE_COROUTINE, "alloc stack: size=%u, ptr=%p", stack_size_, CTX_STACK);

    void *sp = (void *) ((char *) CTX_STACK + stack_size_);
#ifdef USE_VALGRIND
    valgrind_stack_id = VALGRIND_STACK_REGISTER(sp, CTX_STACK);
#endif

#if USE_UCONTEXT
    if (-1 == getcontext(&CTX_CTX)) {
        swoole_throw_error(SW_ERROR_CO_GETCONTEXT_FAILED);
        sw_free(CTX_STACK);
        return;
    }
    CTX_CTX.uc_stack.ss_sp = CTX_STACK;
    CTX_CTX.uc_stack.ss_size = stack_size;
    CTX_CTX.uc_link = nullptr;
    makecontext(&CTX_CTX, (void (*)(void)) & context_func, 1, this);
#else

#if USE_BOOST_V2
    CTX_CTX = make_fcontext(sp, stack_size_, context_func_v2);
    swoole_trace_log(SW_TRACE_COROUTINE, "========v2");
#else
    CTX_CTX = make_fcontext_v1(sp, stack_size_, (void (*)(intptr_t)) & context_func);
    swoole_trace_log(SW_TRACE_COROUTINE, "========v1");
#endif
    CTX_SWAP_CTX = nullptr;
#endif

#ifdef SW_CONTEXT_DETECT_STACK_USAGE
    size_t offset = START_OFFSET;
    while (offset <= stack_size) {
        memcpy((char *) sp - offset + (sizeof(MAGIC_STRING) - 1), SW_STRL(MAGIC_STRING));
        offset *= 2;
    }
#endif

#ifdef SW_CONTEXT_PROTECT_STACK_PAGE
    mprotect(CTX_STACK, SwooleG.pagesize, PROT_NONE);
#endif
}

void Context::native_destroy() {
    if (CTX_STACK) {
        swoole_trace_log(SW_TRACE_COROUTINE, "free stack: ptr=%p", CTX_STACK);
#ifdef USE_VALGRIND
        VALGRIND_STACK_DEREGISTER(valgrind_stack_id);
#endif

#ifdef SW_CONTEXT_PROTECT_STACK_PAGE
        ::munmap(CTX_STACK, stack_size_);
#else
        sw_free(CTX_STACK);
#endif
        CTX_STACK = nullptr;
    }
}

#ifdef SW_CONTEXT_DETECT_STACK_USAGE
ssize_t Context::get_stack_usage() {
    size_t offset = START_OFFSET;
    size_t retval = START_OFFSET;

    void *sp = (void *) ((char *) CTX_STACK + stack_size_);

    while (offset < stack_size_) {
        if (memcmp((char *) sp - offset + (sizeof(MAGIC_STRING) - 1), SW_STRL(MAGIC_STRING)) != 0) {
            retval = offset * 2;
        }
        offset *= 2;
    }

    return retval;
}
#endif

bool Context::native_swap_in() {
#if USE_UCONTEXT
    return 0 == swapcontext(&CTX_SWAP_CTX, &CTX_CTX);
#else

#if USE_BOOST_V2
    transfer_t t = jump_fcontext(CTX_CTX, this);
    CTX_CTX = t.fctx;
#else
    jump_fcontext_v1(&CTX_SWAP_CTX, CTX_CTX, (intptr_t) this, true);
#endif
    return true;
#endif
}

bool Context::native_swap_out() {
#if USE_UCONTEXT
    return 0 == swapcontext(&CTX_CTX, &CTX_SWAP_CTX);
#else
#if USE_BOOST_V2
    transfer_t t = jump_fcontext(CTX_SWAP_CTX, this);
    CTX_SWAP_CTX = t.fctx;
#else
    jump_fcontext_v1(&CTX_CTX, CTX_SWAP_CTX, (intptr_t) this, true);
#endif
    return true;
#endif
}

#if USE_BOOST_V2

void Context::context_func_v2(transfer_t transfer) {
    Context *_this = (Context *) transfer.data;
    _this->CTX_SWAP_CTX = transfer.fctx;
    _this->fn_(_this->private_data_);
    _this->end_ = true;
    _this->native_swap_out();
}

#else

void Context::context_func(void *arg) {
    Context *_this = (Context *) arg;
    _this->fn_(_this->private_data_);
    _this->end_ = true;
    _this->native_swap_out();
}

#endif

}  // namespace coroutine
}  // namespace swoole
#endif
