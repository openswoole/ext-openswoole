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

#include "openswoole_coroutine_context.h"

#if !defined(OSW_USE_THREAD_CONTEXT) && defined(ENABLE_PHP_OPENSWOOLE)

namespace openswoole {
namespace coroutine {

/* Unique kind identifier for OpenSwoole fiber contexts.
 * Tools like xdebug can use this to distinguish OpenSwoole coroutines
 * from native PHP Fibers. */
static char openswoole_fiber_kind_storage;
static void *openswoole_fiber_kind = &openswoole_fiber_kind_storage;

/* Thread-local variable to pass Context* to the fiber entry function
 * during the first context switch. Set in fiber_swap_in() before calling
 * zend_fiber_switch_context(), read in fiber_func(). */
static thread_local Context *switching_context = nullptr;

void Context::fiber_init(size_t stack_size) {
    fiber_.caller_context = nullptr;

    /* Zero-initialize the fiber context struct before calling zend_fiber_init_context.
     * zend_fiber_init_context does NOT initialize all fields (e.g. cleanup is left
     * unset). Without this, zend_fiber_destroy_context could call a garbage cleanup
     * pointer and crash. */
    memset(&fiber_.fiber_context, 0, sizeof(fiber_.fiber_context));

    if (zend_fiber_init_context(&fiber_.fiber_context, openswoole_fiber_kind, fiber_func, stack_size) != SUCCESS) {
        openswoole_fatal_error(OSW_ERROR_MALLOC_FAIL, "failed to init fiber context.");
        exit(254);
    }
    openswoole_trace_log(OSW_TRACE_COROUTINE, "fiber context init: size=%u, ptr=%p", stack_size_, &fiber_.fiber_context);
}

void Context::fiber_destroy() {
    openswoole_trace_log(OSW_TRACE_COROUTINE, "fiber context destroy: ptr=%p, status=%d", &fiber_.fiber_context, fiber_.fiber_context.status);
    /* When fiber_func returns normally, the PHP fiber trampoline sets the status
     * to ZEND_FIBER_STATUS_DEAD and does a final zend_fiber_switch_context. The
     * caller's zend_fiber_switch_context then auto-calls zend_fiber_destroy_context.
     * In that case we must NOT destroy again (double-free). */
    if (fiber_.fiber_context.status != ZEND_FIBER_STATUS_DEAD) {
        zend_fiber_destroy_context(&fiber_.fiber_context);
    }
}

bool Context::fiber_swap_in() {
    fiber_.caller_context = EG(current_fiber_context);
    switching_context = this;

    zend_fiber_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));
    transfer.context = &fiber_.fiber_context;
    ZVAL_NULL(&transfer.value);

    zend_fiber_switch_context(&transfer);
    return true;
}

bool Context::fiber_swap_out() {
    zend_fiber_transfer transfer;
    memset(&transfer, 0, sizeof(transfer));
    transfer.context = fiber_.caller_context;
    ZVAL_NULL(&transfer.value);

    zend_fiber_switch_context(&transfer);
    return true;
}

void Context::fiber_func(zend_fiber_transfer *transfer) {
    Context *_this = switching_context;
    _this->fn_(_this->private_data_);
    _this->end_ = true;

    /* Do NOT call swap_out() here. Instead, return to PHP's fiber trampoline.
     * The trampoline will:
     *   1. Set our fiber status to ZEND_FIBER_STATUS_DEAD
     *   2. Call zend_fiber_switch_context() to switch back to our caller
     *   3. The caller's zend_fiber_switch_context will auto-destroy our context
     *
     * This ensures proper fiber lifecycle (INIT -> RUNNING -> DEAD) with correct
     * observer notifications, and avoids destroying a SUSPENDED fiber which can
     * crash due to uninitialized cleanup callbacks. */
    transfer->context = _this->fiber_.caller_context;
}

}  // namespace coroutine
}  // namespace openswoole
#endif
