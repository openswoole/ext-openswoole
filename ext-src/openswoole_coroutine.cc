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
  | Author: Xinyu Zhu  <xyzhu1120@gmail.com>                             |
  |         shiguangqi <shiguangqi2008@gmail.com>                        |
  |         Twosee  <twose@qq.com>                                       |
  |         Tianfeng Han  <rango@swoole.com>                             |
  +----------------------------------------------------------------------+
 */

#include "php_openswoole_cxx.h"

#include "openswoole_server.h"
#include "openswoole_signal.h"

#include "zend_builtin_functions.h"
#include "ext/spl/spl_array.h"

#include "openswoole_coroutine_arginfo.h"

#include <unordered_map>
#include <chrono>
#include <algorithm>

using std::unordered_map;
using openswoole::Coroutine;
using openswoole::PHPContext;
using openswoole::PHPCoroutine;
using openswoole::coroutine::Channel;
using openswoole::coroutine::Selector;
using openswoole::coroutine::Socket;
using openswoole::coroutine::System;

#define PHP_CORO_TASK_SLOT                                                                                             \
    ((int) ((ZEND_MM_ALIGNED_SIZE(sizeof(PHPContext)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) /                      \
            ZEND_MM_ALIGNED_SIZE(sizeof(zval))))

enum osw_exit_flags { OSW_EXIT_IN_COROUTINE = 1 << 1, OSW_EXIT_IN_SERVER = 1 << 2 };

bool PHPCoroutine::activated = false;
zend_array *PHPCoroutine::options = nullptr;

PHPCoroutine::Config PHPCoroutine::config{
    OSW_DEFAULT_MAX_CORO_NUM,
    0,
    false,
    true,
};

PHPContext PHPCoroutine::main_task{};
std::thread PHPCoroutine::interrupt_thread;
bool PHPCoroutine::interrupt_thread_running = false;

// extern void php_openswoole_load_library();

#if PHP_VERSION_ID >= 80400
static zif_handler ori_exit_handler = nullptr;
#else
static user_opcode_handler_t ori_exit_handler = nullptr;
#endif
static user_opcode_handler_t ori_begin_silence_handler = nullptr;
static user_opcode_handler_t ori_end_silence_handler = nullptr;
static unordered_map<long, Coroutine *> user_yield_coros;

#define ZEND_ERROR_CB_LAST_ARG_D zend_string *message
#define ZEND_ERROR_CB_LAST_ARG_RELAY message

typedef zend_string error_filename_t;

static void (*orig_interrupt_function)(zend_execute_data *execute_data) = nullptr;
static void (*orig_error_function)(int type,
                                   error_filename_t *error_filename,
                                   const uint32_t error_lineno,
                                   ZEND_ERROR_CB_LAST_ARG_D) = nullptr;

static zend_class_entry *openswoole_coroutine_util_ce;
static zend_class_entry *openswoole_exit_exception_ce;
static zend_object_handlers openswoole_exit_exception_handlers;
static zend_class_entry *openswoole_coroutine_iterator_ce;
static zend_class_entry *openswoole_coroutine_context_ce;

OSW_EXTERN_C_BEGIN
static PHP_METHOD(openswoole_coroutine, exists);
static PHP_METHOD(openswoole_coroutine, yield);
static PHP_METHOD(openswoole_coroutine, resume);
static PHP_METHOD(openswoole_coroutine, cancel);
static PHP_METHOD(openswoole_coroutine, isCanceled);
static PHP_METHOD(openswoole_coroutine, stats);
static PHP_METHOD(openswoole_coroutine, getCid);
static PHP_METHOD(openswoole_coroutine, getPcid);
static PHP_METHOD(openswoole_coroutine, getContext);
static PHP_METHOD(openswoole_coroutine, getBackTrace);
static PHP_METHOD(openswoole_coroutine, printBackTrace);
static PHP_METHOD(openswoole_coroutine, getElapsed);
static PHP_METHOD(openswoole_coroutine, getStackUsage);
static PHP_METHOD(openswoole_coroutine, list);
static PHP_METHOD(openswoole_coroutine, enableScheduler);
static PHP_METHOD(openswoole_coroutine, disableScheduler);
static PHP_METHOD(openswoole_coroutine, select);
static PHP_METHOD(openswoole_coroutine, run);
PHP_METHOD(openswoole_coroutine_system, exec);
PHP_METHOD(openswoole_coroutine_system, sleep);
PHP_METHOD(openswoole_coroutine_system, usleep);
PHP_METHOD(openswoole_coroutine_system, fread);
PHP_METHOD(openswoole_coroutine_system, fgets);
PHP_METHOD(openswoole_coroutine_system, fwrite);
PHP_METHOD(openswoole_coroutine_system, statvfs);
PHP_METHOD(openswoole_coroutine_system, getaddrinfo);
PHP_METHOD(openswoole_coroutine_system, readFile);
PHP_METHOD(openswoole_coroutine_system, writeFile);
PHP_METHOD(openswoole_coroutine_system, wait);
PHP_METHOD(openswoole_coroutine_system, waitPid);
PHP_METHOD(openswoole_coroutine_system, waitSignal);
PHP_METHOD(openswoole_coroutine_system, waitEvent);
PHP_METHOD(openswoole_coroutine_system, dnsLookup);
PHP_METHOD(openswoole_coroutine_system, clearDNSCache);
OSW_EXTERN_C_END

// clang-format off

static const zend_function_entry openswoole_coroutine_methods[] =
{
    /**
     * Coroutine Core API
     */
    ZEND_FENTRY(create, ZEND_FN(openswoole_coroutine_create), arginfo_class_OpenSwoole_Coroutine_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(defer, ZEND_FN(openswoole_coroutine_defer), arginfo_class_OpenSwoole_Coroutine_defer, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_scheduler, set, arginfo_class_OpenSwoole_Coroutine_set, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_scheduler, getOptions, arginfo_class_OpenSwoole_Coroutine_getOptions, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    PHP_ME(openswoole_coroutine, exists, arginfo_class_OpenSwoole_Coroutine_exists, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, yield, arginfo_class_OpenSwoole_Coroutine_yield, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, cancel, arginfo_class_OpenSwoole_Coroutine_cancel, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, isCanceled, arginfo_class_OpenSwoole_Coroutine_isCanceled, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, resume, arginfo_class_OpenSwoole_Coroutine_resume, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, stats, arginfo_class_OpenSwoole_Coroutine_stats, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getCid, arginfo_class_OpenSwoole_Coroutine_getCid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getPcid, arginfo_class_OpenSwoole_Coroutine_getPcid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getContext, arginfo_class_OpenSwoole_Coroutine_getContext, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getBackTrace, arginfo_class_OpenSwoole_Coroutine_getBackTrace, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, printBackTrace, arginfo_class_OpenSwoole_Coroutine_printBackTrace, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getElapsed, arginfo_class_OpenSwoole_Coroutine_getElapsed, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, getStackUsage, arginfo_class_OpenSwoole_Coroutine_getStackUsage, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, list, arginfo_class_OpenSwoole_Coroutine_list, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, select, arginfo_class_OpenSwoole_Coroutine_select, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, enableScheduler, arginfo_class_OpenSwoole_Coroutine_enableScheduler, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, disableScheduler, arginfo_class_OpenSwoole_Coroutine_disableScheduler, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine, run, arginfo_class_OpenSwoole_Coroutine_run, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    ZEND_FENTRY(gethostbyname, ZEND_FN(openswoole_coroutine_gethostbyname), arginfo_class_OpenSwoole_Coroutine_gethostbyname, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    PHP_ME(openswoole_coroutine_system, exec, arginfo_class_OpenSwoole_Coroutine_exec, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, sleep, arginfo_class_OpenSwoole_Coroutine_sleep, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, usleep, arginfo_class_OpenSwoole_Coroutine_usleep, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, getaddrinfo, arginfo_class_OpenSwoole_Coroutine_getaddrinfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, statvfs, arginfo_class_OpenSwoole_Coroutine_statvfs, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, readFile, arginfo_class_OpenSwoole_Coroutine_readFile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, writeFile, arginfo_class_OpenSwoole_Coroutine_writeFile, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, wait, arginfo_class_OpenSwoole_Coroutine_wait, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, waitPid, arginfo_class_OpenSwoole_Coroutine_waitPid, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, waitSignal, arginfo_class_OpenSwoole_Coroutine_waitSignal, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, waitEvent, arginfo_class_OpenSwoole_Coroutine_waitEvent, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, dnsLookup, arginfo_class_OpenSwoole_Coroutine_dnsLookup, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(openswoole_coroutine_system, clearDNSCache, arginfo_class_OpenSwoole_Coroutine_clearDNSCache, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
// clang-format on

/**
 * Exit Exception
 */
static PHP_METHOD(openswoole_exit_exception, getFlags);
static PHP_METHOD(openswoole_exit_exception, getStatus);

ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_coroutine_void, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry openswoole_exit_exception_methods[] = {
    PHP_ME(openswoole_exit_exception, getFlags, arginfo_openswoole_coroutine_void, ZEND_ACC_PUBLIC)
        PHP_ME(openswoole_exit_exception, getStatus, arginfo_openswoole_coroutine_void, ZEND_ACC_PUBLIC) PHP_FE_END};

#ifdef ZEND_EXIT
static int coro_exit_handler(zend_execute_data *execute_data) {
    zval ex;
    zend_object *obj;
    zend_long flags = 0;
    if (Coroutine::get_current()) {
        flags |= OSW_EXIT_IN_COROUTINE;
    }
    if (osw_server() && osw_server()->is_started()) {
        flags |= OSW_EXIT_IN_SERVER;
    }
    if (flags) {
        const zend_op *opline = EX(opline);
        zval _exit_status;
        zval *exit_status = nullptr;

        if (opline->op1_type != IS_UNUSED) {
            if (opline->op1_type == IS_CONST) {
                exit_status = RT_CONSTANT(opline, opline->op1);
            } else {
                exit_status = EX_VAR(opline->op1.var);
            }
            if (Z_ISREF_P(exit_status)) {
                exit_status = Z_REFVAL_P(exit_status);
            }
            ZVAL_DUP(&_exit_status, exit_status);
            exit_status = &_exit_status;
        } else {
            exit_status = &_exit_status;
            ZVAL_NULL(exit_status);
        }
        obj = zend_throw_exception(openswoole_exit_exception_ce, "openswoole exit", 0);
        ZVAL_OBJ(&ex, obj);
        zend_update_property_long(openswoole_exit_exception_ce, OSW_Z8_OBJ_P(&ex), ZEND_STRL("flags"), flags);
        Z_TRY_ADDREF_P(exit_status);
        zend_update_property(openswoole_exit_exception_ce, OSW_Z8_OBJ_P(&ex), ZEND_STRL("status"), exit_status);
    }

    return ZEND_USER_OPCODE_DISPATCH;
}
#endif
#if PHP_VERSION_ID >= 80400
/* PHP 8.4+: exit() is a regular function, intercept via function handler replacement */
PHP_FUNCTION(openswoole_exit) {
    zend_long flags = 0;
    if (Coroutine::get_current()) {
        flags |= OSW_EXIT_IN_COROUTINE;
    }
    if (osw_server() && osw_server()->is_started()) {
        flags |= OSW_EXIT_IN_SERVER;
    }

    zend_string *message = NULL;
    zend_long status = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR_OR_LONG(message, status)
    ZEND_PARSE_PARAMETERS_END();

    if (flags) {
        zval ex;
        zend_object *obj = zend_throw_exception(openswoole_exit_exception_ce,
            (message ? ZSTR_VAL(message) : "openswoole exit"), 0);
        ZVAL_OBJ(&ex, obj);
        zend_update_property_long(openswoole_exit_exception_ce, OSW_Z8_OBJ_P(&ex), ZEND_STRL("flags"), flags);
        if (message) {
            zend_update_property_str(openswoole_exit_exception_ce, OSW_Z8_OBJ_P(&ex), ZEND_STRL("status"), message);
        } else {
            zend_update_property_long(openswoole_exit_exception_ce, OSW_Z8_OBJ_P(&ex), ZEND_STRL("status"), status);
        }
    } else {
        /* Not in coroutine/server context, call the original exit() */
        if (ori_exit_handler) {
            ori_exit_handler(INTERNAL_FUNCTION_PARAM_PASSTHRU);
        }
    }
}
#endif

static int coro_begin_silence_handler(zend_execute_data *execute_data) {
    PHPContext *task = PHPCoroutine::get_context();
    task->in_silence = true;
    task->ori_error_reporting = EG(error_reporting);
    return ZEND_USER_OPCODE_DISPATCH;
}

static int coro_end_silence_handler(zend_execute_data *execute_data) {
    PHPContext *task = PHPCoroutine::get_context();
    task->in_silence = false;
    return ZEND_USER_OPCODE_DISPATCH;
}

static void coro_interrupt_resume(void *data) {
    Coroutine *co = (Coroutine *) data;
    if (co && !co->is_end()) {
        openswoole_trace_log(OSW_TRACE_COROUTINE, "interrupt_callback cid=%ld ", co->get_cid());
        co->resume();
    }
}

static void coro_interrupt_function(zend_execute_data *execute_data) {
    PHPContext *task = PHPCoroutine::get_context();
    if (task && task->co && PHPCoroutine::is_schedulable(task)) {
        openswoole_event_defer(coro_interrupt_resume, (void *) task->co);
        task->co->yield();
    }
    if (orig_interrupt_function) {
        orig_interrupt_function(execute_data);
    }
}

void PHPCoroutine::init() {
    Coroutine::set_on_yield(on_yield);
    Coroutine::set_on_resume(on_resume);
    Coroutine::set_on_close(on_close);
}

void PHPCoroutine::activate() {
    if (osw_unlikely(activated)) {
        return;
    }

    /* Apply INI setting for fiber context if not already set via Co::set() */
    if (OPENSWOOLE_G(use_fiber_context)) {
        Coroutine::set_use_fiber_context(true);
    }

    /* init reactor and register event wait */
    php_openswoole_check_reactor();

    /* replace interrupt function */
    orig_interrupt_function = zend_interrupt_function;
    zend_interrupt_function = coro_interrupt_function;

    /* replace the error function to save execute_data */
    orig_error_function = zend_error_cb;
    zend_error_cb =
        [](int type, error_filename_t *error_filename, const uint32_t error_lineno, ZEND_ERROR_CB_LAST_ARG_D) {
            if (osw_unlikely(type & E_FATAL_ERRORS)) {
                if (activated) {
                    /* update the last coroutine's info */
                    save_task(get_context());
                }
                if (osw_reactor()) {
                    osw_reactor()->running = false;
                    osw_reactor()->bailout = true;
                }
#ifdef OSW_EXIT_WHEN_OCCURS_FATAL_ERROR
                zend_try {
                    orig_error_function(type, error_filename, error_lineno, ZEND_ERROR_CB_LAST_ARG_RELAY);
                }
                zend_catch {
                    exit(255);
                }
                zend_end_try();
#endif
            }
            if (osw_likely(orig_error_function)) {
                orig_error_function(type, error_filename, error_lineno, ZEND_ERROR_CB_LAST_ARG_RELAY);
            }
        };

    if (OPENSWOOLE_G(enable_preemptive_scheduler) || config.enable_preemptive_scheduler) {
        /* create a thread to interrupt the coroutine that takes up too much time */
        interrupt_thread_start();
    }

    if (config.hook_flags) {
        enable_hook(config.hook_flags);
    }

    disable_unsafe_function();

    /**
     * deactivate when reactor free.
     */
    osw_reactor()->add_destroy_callback(deactivate, nullptr);
    Coroutine::activate();
    activated = true;
}

void PHPCoroutine::deactivate(void *ptr) {
    interrupt_thread_stop();
    /**
     * reset runtime hook
     */
    disable_hook();

    zend_interrupt_function = orig_interrupt_function;
    zend_error_cb = orig_error_function;

    if (config.enable_deadlock_check) {
        deadlock_check();
    }

    enable_unsafe_function();
    Coroutine::deactivate();
    activated = false;
}

void PHPCoroutine::shutdown() {
    interrupt_thread_stop();
    Coroutine::bailout(nullptr);
    if (options) {
        zend_array_destroy(options);
        options = nullptr;
    }
}

void PHPCoroutine::deadlock_check() {
    if (Coroutine::count() == 0) {
        return;
    }
    if (php_openswoole_is_fatal_error() || (osw_reactor() && osw_reactor()->bailout)) {
        return;
    }

    printf("\n==================================================================="
           "\n [FATAL ERROR]: all coroutines (count: %lu) are asleep - deadlock!"
           "\n===================================================================\n",
           Coroutine::count());
}

void PHPCoroutine::interrupt_thread_stop() {
    if (!interrupt_thread_running) {
        return;
    }
    interrupt_thread_running = false;
    interrupt_thread.join();
}

void PHPCoroutine::interrupt_thread_start() {
    if (interrupt_thread_running) {
        return;
    }
    interrupt_thread_running = true;
    interrupt_thread = std::thread([]() {
        openswoole_signal_block_all();
        while (interrupt_thread_running) {
            zend_atomic_bool_store_ex(&EG(vm_interrupt), true);
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_EXEC_MSEC / 2));
        }
    });
}

inline void PHPCoroutine::vm_stack_init(void) {
    uint32_t size = OSW_DEFAULT_PHP_STACK_PAGE_SIZE;
    zend_vm_stack page = (zend_vm_stack) emalloc(size);

    page->top = ZEND_VM_STACK_ELEMENTS(page);
    page->end = (zval *) ((char *) page + size);
    page->prev = nullptr;

    EG(vm_stack) = page;
    EG(vm_stack)->top++;
    EG(vm_stack_top) = EG(vm_stack)->top;
    EG(vm_stack_end) = EG(vm_stack)->end;
    EG(vm_stack_page_size) = size;
}

inline void PHPCoroutine::vm_stack_destroy(void) {
    zend_vm_stack stack = EG(vm_stack);

    while (stack != nullptr) {
        zend_vm_stack p = stack->prev;
        efree(stack);
        stack = p;
    }
}

/**
 * The meaning of the task argument in coro switch functions
 *
 * create: origin_task
 * yield: current_task
 * resume: target_task
 * close: current_task
 *
 */
inline void PHPCoroutine::save_vm_stack(PHPContext *task) {
#ifdef OSW_CORO_SWAP_BAILOUT
    task->bailout = EG(bailout);
#endif
    task->vm_stack_top = EG(vm_stack_top);
    task->vm_stack_end = EG(vm_stack_end);
    task->vm_stack = EG(vm_stack);
    task->vm_stack_page_size = EG(vm_stack_page_size);
    task->execute_data = EG(current_execute_data);
    task->jit_trace_num = EG(jit_trace_num);
#ifdef ZEND_CHECK_STACK_LIMIT
    task->stack_base = EG(stack_base);
    task->stack_limit = EG(stack_limit);
#endif
    task->error_handling = EG(error_handling);
    task->exception_class = EG(exception_class);
    task->exception = EG(exception);
    if (!Coroutine::use_fiber_context) {
        // With fiber context, zend_fiber_switch_context manages EG(error_reporting).
        // Do not modify it here to avoid conflicts.
        if (UNEXPECTED(task->in_silence)) {
            task->tmp_error_reporting = EG(error_reporting);
            EG(error_reporting) = task->ori_error_reporting;
        }
    }
}

inline void PHPCoroutine::restore_vm_stack(PHPContext *task) {
    if (!Coroutine::use_fiber_context) {
        // With fiber context, zend_fiber_switch_context() saves/restores these fields
        // automatically. Restoring them here would conflict and corrupt VM state.
#ifdef OSW_CORO_SWAP_BAILOUT
        EG(bailout) = task->bailout;
#endif
        EG(vm_stack_top) = task->vm_stack_top;
        EG(vm_stack_end) = task->vm_stack_end;
        EG(vm_stack) = task->vm_stack;
        EG(vm_stack_page_size) = task->vm_stack_page_size;
        EG(current_execute_data) = task->execute_data;
        EG(jit_trace_num) = task->jit_trace_num;
#ifdef ZEND_CHECK_STACK_LIMIT
        EG(stack_base) = task->stack_base;
        EG(stack_limit) = task->stack_limit;
#endif
        if (UNEXPECTED(task->in_silence)) {
            EG(error_reporting) = task->tmp_error_reporting;
        }
    }
    EG(error_handling) = task->error_handling;
    EG(exception_class) = task->exception_class;
    EG(exception) = task->exception;
}

inline void PHPCoroutine::save_og(PHPContext *task) {
    if (OG(handlers).elements) {
        task->output_ptr = (zend_output_globals *) emalloc(sizeof(zend_output_globals));
        memcpy(task->output_ptr, SWOG, sizeof(zend_output_globals));
        php_output_activate();
    } else {
        task->output_ptr = nullptr;
    }
}

inline void PHPCoroutine::restore_og(PHPContext *task) {
    if (task->output_ptr) {
        memcpy(SWOG, task->output_ptr, sizeof(zend_output_globals));
        efree(task->output_ptr);
        task->output_ptr = nullptr;
    }
}

void PHPCoroutine::set_hook_flags(uint32_t flags) {
    zval options;
    array_init(&options);
    add_assoc_long(&options, "hook_flags", flags);

    if (PHPCoroutine::options) {
        zend_hash_merge(PHPCoroutine::options, Z_ARRVAL(options), nullptr, true);
        zval_ptr_dtor(&options);
    } else {
        PHPCoroutine::options = Z_ARRVAL(options);
    }

    config.hook_flags = flags;
}

void PHPCoroutine::save_task(PHPContext *task) {
    save_vm_stack(task);
    save_og(task);
}

void PHPCoroutine::restore_task(PHPContext *task) {
    restore_vm_stack(task);
    restore_og(task);
}

void PHPCoroutine::on_yield(void *arg) {
    PHPContext *task = (PHPContext *) arg;
    PHPContext *origin_task = get_origin_context(task);
    openswoole_trace_log(
        OSW_TRACE_COROUTINE, "php_coro_yield from cid=%ld to cid=%ld", task->co->get_cid(), task->co->get_origin_cid());
    save_task(task);
    restore_task(origin_task);
}

void PHPCoroutine::on_resume(void *arg) {
    PHPContext *task = (PHPContext *) arg;
    PHPContext *current_task = get_context();
    save_task(current_task);
    restore_task(task);
    record_last_msec(task);
    openswoole_trace_log(OSW_TRACE_COROUTINE,
                     "php_coro_resume from cid=%ld to cid=%ld",
                     Coroutine::get_current_cid(),
                     task->co->get_cid());
}

void PHPCoroutine::on_close(void *arg) {
    PHPContext *task = (PHPContext *) arg;
    PHPContext *origin_task = get_origin_context(task);
#ifdef OSW_LOG_TRACE_OPEN
    // MUST be assigned here, the task memory may have been released
    long cid = task->co->get_cid();
    long origin_cid = task->co->get_origin_cid();
#endif

    if (OpenSwooleG.hooks[OSW_GLOBAL_HOOK_ON_CORO_STOP]) {
        openswoole_call_hook(OSW_GLOBAL_HOOK_ON_CORO_STOP, task);
    }

    if (OG(handlers).elements) {
        zend_bool no_headers = SG(request_info).no_headers;
        /* Do not send headers by SAPI */
        SG(request_info).no_headers = 1;
        if (OG(active)) {
            php_output_end_all();
        }
        php_output_deactivate();
        php_output_activate();
        SG(request_info).no_headers = no_headers;
    }

    if (OpenSwooleG.max_concurrency > 0 && task->pcid == -1) {
        OpenSwooleWG.worker_concurrency--;
    }
    if (Coroutine::use_fiber_context) {
        // With fiber context, EG(vm_stack) is the caller's (restored by zend_fiber_switch_context).
        // Destroy the coroutine's VM stack using the pointer saved in the task struct.
        zend_vm_stack stack = task->vm_stack;
        while (stack != nullptr) {
            zend_vm_stack p = stack->prev;
            efree(stack);
            stack = p;
        }
    } else {
        vm_stack_destroy();
    }
    restore_task(origin_task);

    openswoole_trace_log(OSW_TRACE_COROUTINE,
                     "coro close cid=%ld and resume to %ld, %zu remained. usage size: %zu. malloc size: %zu",
                     cid,
                     origin_cid,
                     (uintmax_t) Coroutine::count() - 1,
                     (uintmax_t) zend_memory_usage(0),
                     (uintmax_t) zend_memory_usage(1));
}

void PHPCoroutine::main_func(void *arg) {
#ifdef OSW_CORO_SUPPORT_BAILOUT
    zend_first_try {
#endif
        int i;
        Args *php_arg = (Args *) arg;
        zend_fcall_info_cache fci_cache = *php_arg->fci_cache;
        zend_function *func = fci_cache.function_handler;
        zval *argv = php_arg->argv;
        int argc = php_arg->argc;
        PHPContext *task;
        zend_execute_data *call;
        zval _retval, *retval = &_retval;

        if (fci_cache.object) {
            GC_ADDREF(fci_cache.object);
        }

        vm_stack_init();
        call = (zend_execute_data *) (EG(vm_stack_top));
        task = (PHPContext *) EG(vm_stack_top);
        EG(vm_stack_top) = (zval *) ((char *) call + PHP_CORO_TASK_SLOT * sizeof(zval));

    do {
        uint32_t call_info;
        void *object_or_called_scope;
        if ((func->common.fn_flags & ZEND_ACC_STATIC) || !fci_cache.object) {
            object_or_called_scope = fci_cache.called_scope;
            call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC;
        } else {
            object_or_called_scope = fci_cache.object;
            call_info = ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC | ZEND_CALL_HAS_THIS;
        }
        call = zend_vm_stack_push_call_frame(call_info, func, argc, object_or_called_scope);
    } while (0);

        for (i = 0; i < argc; ++i) {
            zval *param;
            zval *arg = &argv[i];
            if (Z_ISREF_P(arg) && !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
                /* don't separate references for __call */
                arg = Z_REFVAL_P(arg);
            }
            param = ZEND_CALL_ARG(call, i + 1);
            ZVAL_COPY(param, arg);
        }

        call->symbol_table = nullptr;

        if (func->op_array.fn_flags & ZEND_ACC_CLOSURE) {
            uint32_t call_info;
            GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
            call_info = ZEND_CALL_CLOSURE;
            ZEND_ADD_CALL_FLAG(call, call_info);
        }

#if defined(OSW_CORO_SWAP_BAILOUT) && !defined(OSW_CORO_SUPPORT_BAILOUT)
        EG(bailout) = nullptr;
#endif
        EG(current_execute_data) = call;
        EG(error_handling) = EH_NORMAL;
        EG(exception_class) = nullptr;
        EG(exception) = nullptr;
        EG(jit_trace_num) = 0;

        task->output_ptr = nullptr;
        task->in_silence = false;

        task->co = Coroutine::get_current();
        task->co->set_task((void *) task);
        task->defer_tasks = nullptr;
        task->pcid = task->co->get_origin_cid();
        task->context = nullptr;
        task->enable_scheduler = true;

#ifdef ZEND_CHECK_STACK_LIMIT
        if (!Coroutine::use_fiber_context) {
            // Fiber context: zend_fiber_switch_context() sets EG(stack_base/stack_limit) automatically.
            // Other backends: we must set them manually from the coroutine stack.
            if(task->co) {
                EG(stack_base) = (void*)((uintptr_t)task->co->get_context().get_stack() + task->co->get_context().get_stack_size());
                zend_ulong reserve = EG(reserved_stack_size);
            #ifdef __APPLE__
                reserve = reserve * 2;
            #endif
                EG(stack_limit) = (int8_t*)task->co->get_context().get_stack() + reserve;
            } else {
                EG(stack_base) = nullptr;
                EG(stack_limit) = nullptr;
            }
        }
#endif
        save_vm_stack(task);
        record_last_msec(task);

        openswoole_trace_log(OSW_TRACE_COROUTINE,
                         "Create coro id: %ld, origin cid: %ld, coro total count: %zu, heap size: %zu",
                         task->co->get_cid(),
                         task->co->get_origin_cid(),
                         (uintmax_t) Coroutine::count(),
                         (uintmax_t) zend_memory_usage(0));

        if (OpenSwooleG.max_concurrency > 0 && task->pcid == -1) {
            // wait until concurrency slots are available
            while (OpenSwooleWG.worker_concurrency > OpenSwooleG.max_concurrency - 1) {
                openswoole_trace_log(OSW_TRACE_COROUTINE,
                                 "php_coro cid=%ld waiting for concurrency slots: max: %d, used: %d",
                                 task->co->get_cid(),
                                 OpenSwooleG.max_concurrency,
                                 OpenSwooleWG.worker_concurrency);

                openswoole_event_defer(
                    [](void *data) {
                        Coroutine *co = (Coroutine *) data;
                        co->resume();
                    },
                    (void *) task->co);
                task->co->yield();
            }
            OpenSwooleWG.worker_concurrency++;
        }

        if (OpenSwooleG.hooks[OSW_GLOBAL_HOOK_ON_CORO_START]) {
            openswoole_call_hook(OSW_GLOBAL_HOOK_ON_CORO_START, task);
        }

        if (EXPECTED(func->type == ZEND_USER_FUNCTION)) {
            ZVAL_UNDEF(retval);
            // TODO: enhancement it, separate execute data is necessary, but we lose the backtrace
            EG(current_execute_data) = nullptr;
            zend_init_func_execute_data(call, &func->op_array, retval);
            zend_execute_ex(EG(current_execute_data));
        } else /* ZEND_INTERNAL_FUNCTION */
        {
            ZVAL_NULL(retval);
            call->prev_execute_data = nullptr;
            call->return_value = nullptr; /* this is not a constructor call */
            execute_internal(call, retval);
            zend_vm_stack_free_args(call);
        }

        if (task->defer_tasks) {
            std::stack<zend::Function *> *tasks = task->defer_tasks;
            while (!tasks->empty()) {
                zend::Function *defer_fci = tasks->top();
                tasks->pop();
                if (Z_TYPE_P(retval) != IS_UNDEF) {
                    defer_fci->fci.param_count = 1;
                    defer_fci->fci.params = retval;
                }

                if (UNEXPECTED(osw_zend_call_function_anyway(&defer_fci->fci, &defer_fci->fci_cache) != SUCCESS)) {
                    php_openswoole_fatal_error(E_WARNING, "defer callback handler error");
                }
                osw_zend_fci_cache_discard(&defer_fci->fci_cache);
                efree(defer_fci);
            }
            delete task->defer_tasks;
            task->defer_tasks = nullptr;
        }

        // resources release
        if (task->context) {
            zend_object *context = task->context;
            task->context = (zend_object *) ~0;
            OBJ_RELEASE(context);
        }
        if (fci_cache.object) {
            OBJ_RELEASE(fci_cache.object);
        }
        zval_ptr_dtor(retval);

        if (Coroutine::use_fiber_context) {
            // Save the coroutine's final VM stack state so on_close can properly
            // destroy it. After main_func returns, fiber_func returns to the PHP
            // fiber trampoline which does the final context switch. The coroutine's
            // EG(vm_stack) is only accessible via this saved pointer in on_close.
            save_vm_stack(task);
        }

        if (UNEXPECTED(EG(exception))) {
            zend_exception_error(EG(exception), E_ERROR);
            // TODO: php8 don't exit on exceptions, but no reason to continue, fix this in the future
            // Keep the behavior the same as php7
            zend_bailout();  // exit for php8
        }

#ifdef OSW_CORO_SUPPORT_BAILOUT
    }
    zend_catch {
        Coroutine::bailout([]() {
            if (osw_reactor()) {
                osw_reactor()->running = false;
                osw_reactor()->bailout = true;
            }
            osw_zend_bailout();
        });
    }
    zend_end_try();
#endif
}

long PHPCoroutine::create(zend_fcall_info_cache *fci_cache, uint32_t argc, zval *argv) {
    if (osw_unlikely(Coroutine::count() >= config.max_num)) {
        php_openswoole_fatal_error(E_WARNING, "exceed max number of coroutine %zu", (uintmax_t) Coroutine::count());
        return Coroutine::ERR_LIMIT;
    }
    if (osw_unlikely(!fci_cache || !fci_cache->function_handler)) {
        php_openswoole_fatal_error(E_ERROR, "invalid function call info cache");
        return Coroutine::ERR_INVALID;
    }
    zend_uchar type = fci_cache->function_handler->type;
    if (osw_unlikely(type != ZEND_USER_FUNCTION && type != ZEND_INTERNAL_FUNCTION)) {
        php_openswoole_fatal_error(E_ERROR, "invalid function type %u", fci_cache->function_handler->type);
        return Coroutine::ERR_INVALID;
    }

    if (osw_unlikely(!activated)) {
        activate();
    }

    Args _args;
    _args.fci_cache = fci_cache;
    _args.argv = argv;
    _args.argc = argc;
    save_task(get_context());

    return Coroutine::create(main_func, (void *) &_args);
}

void PHPCoroutine::defer(zend::Function *fci) {
    PHPContext *task = get_context();
    if (task->defer_tasks == nullptr) {
        task->defer_tasks = new std::stack<zend::Function *>;
    }
    task->defer_tasks->push(fci);
}

void php_openswoole_coroutine_minit(int module_number) {
    PHPCoroutine::init();

    OSW_INIT_CLASS_ENTRY_BASE(
        openswoole_coroutine_util, "OpenSwoole\\Coroutine", nullptr, "Co", openswoole_coroutine_methods, nullptr);
    OSW_SET_CLASS_CREATE(openswoole_coroutine_util, osw_zend_create_object_deny);

    zend_declare_class_constant_long(
        openswoole_coroutine_util_ce, ZEND_STRL("DEFAULT_MAX_CORO_NUM"), OSW_DEFAULT_MAX_CORO_NUM);
    zend_declare_class_constant_long(
        openswoole_coroutine_util_ce, ZEND_STRL("CORO_MAX_NUM_LIMIT"), Coroutine::MAX_NUM_LIMIT);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("CORO_INIT"), Coroutine::STATE_INIT);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("CORO_WAITING"), Coroutine::STATE_WAITING);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("CORO_RUNNING"), Coroutine::STATE_RUNNING);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("CORO_END"), Coroutine::STATE_END);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("EXIT_IN_COROUTINE"), OSW_EXIT_IN_COROUTINE);
    zend_declare_class_constant_long(openswoole_coroutine_util_ce, ZEND_STRL("EXIT_IN_SERVER"), OSW_EXIT_IN_SERVER);

    // backward compatibility
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_DEFAULT_MAX_CORO_NUM", OSW_DEFAULT_MAX_CORO_NUM);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CORO_MAX_NUM_LIMIT", Coroutine::MAX_NUM_LIMIT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CORO_INIT", Coroutine::STATE_INIT);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CORO_WAITING", Coroutine::STATE_WAITING);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CORO_RUNNING", Coroutine::STATE_RUNNING);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_CORO_END", Coroutine::STATE_END);

    OSW_INIT_CLASS_ENTRY_BASE(
        openswoole_coroutine_iterator, "OpenSwoole\\Coroutine\\Iterator", nullptr, nullptr, nullptr, spl_ce_ArrayIterator);
    OSW_INIT_CLASS_ENTRY_BASE(
        openswoole_coroutine_context, "OpenSwoole\\Coroutine\\Context", nullptr, nullptr, nullptr, spl_ce_ArrayObject);

    // prohibit exit in coroutine
    OSW_INIT_CLASS_ENTRY_EX(openswoole_exit_exception,
                           "OpenSwoole\\ExitException",
                           nullptr,
                           nullptr,
                           openswoole_exit_exception_methods,
                           openswoole_exception);
    zend_declare_property_long(openswoole_exit_exception_ce, ZEND_STRL("flags"), 0, ZEND_ACC_PRIVATE);
    zend_declare_property_long(openswoole_exit_exception_ce, ZEND_STRL("status"), 0, ZEND_ACC_PRIVATE);
}

void php_openswoole_coroutine_rinit() {
    if (OPENSWOOLE_G(cli)) {

#ifdef ZEND_EXIT
        ori_exit_handler = zend_get_user_opcode_handler(ZEND_EXIT);
        zend_set_user_opcode_handler(ZEND_EXIT, coro_exit_handler);
#endif
#if PHP_VERSION_ID >= 80400
        /* PHP 8.4+: exit() is a regular function, replace its handler */
        zend_function *exit_fn = (zend_function *) zend_hash_str_find_ptr(EG(function_table), ZEND_STRL("exit"));
        if (exit_fn) {
            ori_exit_handler = exit_fn->internal_function.handler;
            exit_fn->internal_function.handler = PHP_FN(openswoole_exit);
        }
#endif
        ori_begin_silence_handler = zend_get_user_opcode_handler(ZEND_BEGIN_SILENCE);
        zend_set_user_opcode_handler(ZEND_BEGIN_SILENCE, coro_begin_silence_handler);

        ori_end_silence_handler = zend_get_user_opcode_handler(ZEND_END_SILENCE);
        zend_set_user_opcode_handler(ZEND_END_SILENCE, coro_end_silence_handler);
    }
}

void php_openswoole_coroutine_rshutdown() {
#if PHP_VERSION_ID >= 80400
    /* Restore original exit() handler */
    if (ori_exit_handler) {
        zend_function *exit_fn = (zend_function *) zend_hash_str_find_ptr(EG(function_table), ZEND_STRL("exit"));
        if (exit_fn) {
            exit_fn->internal_function.handler = ori_exit_handler;
        }
        ori_exit_handler = nullptr;
    }
#endif
    PHPCoroutine::shutdown();
}

static PHP_METHOD(openswoole_exit_exception, getFlags) {
    OSW_RETURN_PROPERTY("flags");
}

static PHP_METHOD(openswoole_exit_exception, getStatus) {
    OSW_RETURN_PROPERTY("status");
}

PHP_FUNCTION(openswoole_coroutine_create) {
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_FUNC(fci, fci_cache)
    Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (osw_unlikely(OPENSWOOLE_G(req_status) == PHP_OPENSWOOLE_CALL_USER_SHUTDOWNFUNC_BEGIN)) {
        zend_function *func = (zend_function *) EG(current_execute_data)->prev_execute_data->func;
        if (func->common.function_name &&
            osw_unlikely(memcmp(ZSTR_VAL(func->common.function_name), ZEND_STRS("__destruct")) == 0)) {
            php_openswoole_fatal_error(E_ERROR, "can not use coroutine in __destruct after php_request_shutdown");
            RETURN_FALSE;
        }
    }

    long cid = PHPCoroutine::create(&fci_cache, fci.param_count, fci.params);
    if (osw_likely(cid > 0)) {
        RETURN_LONG(cid);
    } else {
        RETURN_FALSE;
    }
}

PHP_FUNCTION(openswoole_coroutine_defer) {
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;
    zend::Function *defer_fci;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_FUNC(fci, fci_cache)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Coroutine::get_current_safe();
    defer_fci = (zend::Function *) emalloc(sizeof(zend::Function));
    defer_fci->fci = fci;
    defer_fci->fci_cache = fci_cache;
    osw_zend_fci_cache_persist(&defer_fci->fci_cache);
    PHPCoroutine::defer(defer_fci);
}

static PHP_METHOD(openswoole_coroutine, stats) {
    array_init(return_value);
    add_assoc_long_ex(return_value, ZEND_STRL("event_num"), osw_reactor() ? osw_reactor()->get_event_num() : 0);
    add_assoc_long_ex(
        return_value, ZEND_STRL("signal_listener_num"), OpenSwooleTG.signal_listener_num + OpenSwooleTG.co_signal_listener_num);

    if (OpenSwooleTG.async_threads) {
        add_assoc_long_ex(return_value, ZEND_STRL("aio_task_num"), OpenSwooleTG.async_threads->task_num);
        add_assoc_long_ex(return_value, ZEND_STRL("aio_worker_num"), OpenSwooleTG.async_threads->thread_count());
    } else {
        add_assoc_long_ex(return_value, ZEND_STRL("aio_task_num"), 0);
        add_assoc_long_ex(return_value, ZEND_STRL("aio_worker_num"), 0);
    }
    add_assoc_long_ex(return_value, ZEND_STRL("c_stack_size"), Coroutine::get_stack_size());
    add_assoc_long_ex(return_value, ZEND_STRL("coroutine_num"), Coroutine::count());
    add_assoc_long_ex(return_value, ZEND_STRL("coroutine_peak_num"), Coroutine::get_peak_num());
    add_assoc_long_ex(return_value, ZEND_STRL("coroutine_last_cid"), Coroutine::get_last_cid());
    add_assoc_bool_ex(return_value, ZEND_STRL("use_fiber_context"), Coroutine::use_fiber_context);
}

static PHP_METHOD(openswoole_coroutine, run) {
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    ZEND_PARSE_PARAMETERS_START(1, -1)
    Z_PARAM_FUNC(fci, fci_cache)
    Z_PARAM_VARIADIC('*', fci.params, fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (!PHPCoroutine::get_hook_flags()) {
        PHPCoroutine::set_hook_flags(PHPCoroutine::HOOK_ALL);
    }

    if (OpenSwooleTG.reactor) {
        php_openswoole_fatal_error(
            E_WARNING, "eventLoop has already been created. unable to start %s", OSW_Z_OBJCE_NAME_VAL_P(ZEND_THIS));
        RETURN_FALSE;
    }
    if (php_openswoole_reactor_init() < 0) {
        RETURN_FALSE;
    }

    PHPCoroutine::create(&fci_cache, fci.param_count, fci.params);
    php_openswoole_event_wait();
    RETURN_TRUE;
}

PHP_METHOD(openswoole_coroutine, getCid) {
    RETURN_LONG(PHPCoroutine::get_cid());
}

PHP_METHOD(openswoole_coroutine, getPcid) {
    zend_long cid = 0;
    zend_long ret;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    ret = PHPCoroutine::get_pcid(cid);
    if (ret == 0) {
        RETURN_FALSE;
    }

    RETURN_LONG(ret);
}

static PHP_METHOD(openswoole_coroutine, getContext) {
    zend_long cid = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    PHPContext *task =
        (PHPContext *) (EXPECTED(cid == 0) ? Coroutine::get_current_task() : Coroutine::get_task_by_cid(cid));
    if (UNEXPECTED(!task)) {
        openswoole_set_last_error(OSW_ERROR_CO_NOT_EXISTS);
        RETURN_NULL();
    }
    if (UNEXPECTED(task->context == (zend_object *) ~0)) {
        php_openswoole_fatal_error(E_WARNING, "Context of this coroutine has been destroyed");
        RETURN_NULL();
    }
    if (UNEXPECTED(!task->context)) {
        object_init_ex(return_value, openswoole_coroutine_context_ce);
        task->context = Z_OBJ_P(return_value);
    }
    GC_ADDREF(task->context);
    RETURN_OBJ(task->context);
}

static PHP_METHOD(openswoole_coroutine, getElapsed) {
    zend_long cid = 0;
    zend_long ret;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    ret = PHPCoroutine::get_elapsed(cid);
    RETURN_LONG(ret);
}

static PHP_METHOD(openswoole_coroutine, getStackUsage) {
    zend_long current_cid = PHPCoroutine::get_cid();
    zend_long cid = current_cid;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    PHPContext *task = (PHPContext *) PHPCoroutine::get_context_by_cid(cid);
    if (UNEXPECTED(!task)) {
        openswoole_set_last_error(OSW_ERROR_CO_NOT_EXISTS);
        RETURN_FALSE;
    }

    zend_vm_stack stack = cid == current_cid ? EG(vm_stack) : task->vm_stack;
    size_t usage = 0;

    while (stack) {
        usage += (stack->end - stack->top) * sizeof(zval);
        stack = stack->prev;
    }

    RETURN_LONG(usage);
}

static PHP_METHOD(openswoole_coroutine, exists) {
    zend_long cid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(Coroutine::get_by_cid(cid) != nullptr);
}

static PHP_METHOD(openswoole_coroutine, resume) {
    long cid;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &cid) == FAILURE) {
        RETURN_FALSE;
    }

    auto coroutine_iterator = user_yield_coros.find(cid);
    if (coroutine_iterator == user_yield_coros.end()) {
        php_openswoole_fatal_error(E_WARNING, "you can not resume the coroutine which is in IO operation or non-existent");
        RETURN_FALSE;
    }

    Coroutine *co = coroutine_iterator->second;
    user_yield_coros.erase(cid);
    co->resume();

    RETURN_TRUE;
}

static PHP_METHOD(openswoole_coroutine, yield) {
    Coroutine *co = Coroutine::get_current_safe();
    user_yield_coros[co->get_cid()] = co;

    Coroutine::CancelFunc cancel_fn = [](Coroutine *co) {
        user_yield_coros.erase(co->get_cid());
        co->resume();
        return true;
    };
    co->yield(&cancel_fn);
    if (co->is_canceled()) {
        openswoole_set_last_error(OSW_ERROR_CO_CANCELED);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static PHP_METHOD(openswoole_coroutine, cancel) {
    long cid;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &cid) == FAILURE) {
        RETURN_FALSE;
    }

    Coroutine *co = openswoole_coroutine_get(cid);
    if (!co) {
        openswoole_set_last_error(OSW_ERROR_CO_NOT_EXISTS);
        RETURN_FALSE;
    }
    RETURN_BOOL(co->cancel());
}

static PHP_METHOD(openswoole_coroutine, isCanceled) {
    Coroutine *co = Coroutine::get_current_safe();
    RETURN_BOOL(co->is_canceled());
}

PHP_FUNCTION(openswoole_test_kernel_coroutine) {
    if (!PHPCoroutine::is_activated()) {
        RETURN_FALSE;
    }

    zend_long count = 100;
    double sleep_time = 1.0;

    ZEND_PARSE_PARAMETERS_START(0, 2)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(count)
    Z_PARAM_DOUBLE(sleep_time)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Coroutine::create([=](void *ptr) {
        OSW_LOOP_N(count) {
            System::sleep(sleep_time);
        }
    });
}

static PHP_METHOD(openswoole_coroutine, getBackTrace) {
    zend_long cid = 0;
    zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
    zend_long limit = 0;

    ZEND_PARSE_PARAMETERS_START(0, 3)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    Z_PARAM_LONG(options)
    Z_PARAM_LONG(limit)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (!cid || cid == PHPCoroutine::get_cid()) {
        zend_fetch_debug_backtrace(return_value, 0, options, limit);
    } else {
        PHPContext *task = (PHPContext *) PHPCoroutine::get_context_by_cid(cid);
        if (UNEXPECTED(!task)) {
            openswoole_set_last_error(OSW_ERROR_CO_NOT_EXISTS);
            RETURN_FALSE;
        }
        zend_execute_data *ex_backup = EG(current_execute_data);
        EG(current_execute_data) = task->execute_data;
        zend_fetch_debug_backtrace(return_value, 0, options, limit);
        EG(current_execute_data) = ex_backup;
    }
}

static PHP_METHOD(openswoole_coroutine, printBackTrace) {
    zend_long cid = 0;
    zend_long options = DEBUG_BACKTRACE_PROVIDE_OBJECT;
    zend_long limit = 0;

    ZEND_PARSE_PARAMETERS_START(0, 3)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(cid)
    Z_PARAM_LONG(options)
    Z_PARAM_LONG(limit)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zval argv[2];
    ZVAL_LONG(&argv[0], options);
    ZVAL_LONG(&argv[1], limit);

    if (!cid || cid == PHPCoroutine::get_cid()) {
        zend::function::call("debug_print_backtrace", 2, argv);
    } else {
        PHPContext *task = (PHPContext *) PHPCoroutine::get_context_by_cid(cid);
        if (UNEXPECTED(!task)) {
            openswoole_set_last_error(OSW_ERROR_CO_NOT_EXISTS);
            RETURN_FALSE;
        }
        zend_execute_data *ex_backup = EG(current_execute_data);
        EG(current_execute_data) = task->execute_data;
        zend::function::call("debug_print_backtrace", 2, argv);
        EG(current_execute_data) = ex_backup;
    }
}

static PHP_METHOD(openswoole_coroutine, list) {
    zval zlist;
    array_init(&zlist);
    for (auto &co : Coroutine::coroutines) {
        add_next_index_long(&zlist, co.second->get_cid());
    }
    object_init_ex(return_value, openswoole_coroutine_iterator_ce);
    osw_zend_call_method_with_1_params(return_value,
                                      openswoole_coroutine_iterator_ce,
                                      &openswoole_coroutine_iterator_ce->constructor,
                                      "__construct",
                                      nullptr,
                                      &zlist);
    zval_ptr_dtor(&zlist);
}

struct ChannelObject {
    Channel *chan;
    zend_object std;
};

static osw_inline ChannelObject *php_openswoole_channel_coro_fetch_object(zend_object *obj) {
    return (ChannelObject *) ((char *) obj - XtOffsetOf(ChannelObject, std));
}

static osw_inline Channel *php_openswoole_get_channel(zval *zobject) {
    Channel *chan = php_openswoole_channel_coro_fetch_object(Z_OBJ_P(zobject))->chan;
    return chan;
}

static PHP_METHOD(openswoole_coroutine, select) {
    zval *pull_chans = nullptr;
    zval *push_chans = nullptr;
    double timeout = -1;
    ZEND_PARSE_PARAMETERS_START(2, 3)
    Z_PARAM_ARRAY_EX(pull_chans, 0, 1)
    Z_PARAM_ARRAY_EX(push_chans, 0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (timeout == -1) {
        timeout = INT_MAX;
    }

    if (timeout <= 0 || timeout > INT_MAX) {
        timeout = INT_MAX;
    }

    zend_ulong num_idx;
    zend_string *key;
    zval *val;
    (void) key;

    std::vector<Channel *> pull_chans_vector;
    std::vector<Channel *> push_chans_vector;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(pull_chans), num_idx, key, val) {
        ZVAL_DEREF(val);
        Channel *chan = php_openswoole_get_channel(val);
        pull_chans_vector.push_back(chan);
    }
    ZEND_HASH_FOREACH_END();

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(push_chans), num_idx, key, val) {
        ZVAL_DEREF(val);
        Channel *chan = php_openswoole_get_channel(val);
        push_chans_vector.push_back(chan);
    }
    ZEND_HASH_FOREACH_END();

    Selector selector;
    std::pair<std::vector<int>, std::vector<int>> ready =
        selector.select(pull_chans_vector, push_chans_vector, timeout);

    HashTable *h1 = Z_ARRVAL_P(pull_chans);
    ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(pull_chans), num_idx, key) {
        ZVAL_DEREF(val);
        if (std::find(ready.first.begin(), ready.first.end(), num_idx) == ready.first.end()) {
            zend_hash_index_del(h1, num_idx);
        }
    }
    ZEND_HASH_FOREACH_END();

    HashTable *h2 = Z_ARRVAL_P(push_chans);
    ZEND_HASH_FOREACH_KEY(Z_ARRVAL_P(push_chans), num_idx, key) {
        ZVAL_DEREF(val);
        if (std::find(ready.second.begin(), ready.second.end(), num_idx) == ready.second.end()) {
            zend_hash_index_del(h2, num_idx);
        }
    }
    ZEND_HASH_FOREACH_END();

    array_init(return_value);
    add_assoc_zval_ex(return_value, ZEND_STRL("read"), pull_chans);
    Z_TRY_ADDREF_P(pull_chans);
    add_assoc_zval_ex(return_value, ZEND_STRL("write"), push_chans);
    Z_TRY_ADDREF_P(push_chans);
}

PHP_METHOD(openswoole_coroutine, enableScheduler) {
    RETURN_BOOL(PHPCoroutine::enable_scheduler());
}

PHP_METHOD(openswoole_coroutine, disableScheduler) {
    RETURN_BOOL(PHPCoroutine::disable_scheduler());
}

/**
 * for gdb
 */
zend_executor_globals *php_openswoole_get_executor_globals() {
    return (zend_executor_globals *) &EG(uninitialized_zval);
}
