/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2012-2015 The Swoole Group                             |
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

#include "php_openswoole_cxx.h"
#include "openswoole_server.h"
#include "ext/spl/spl_array.h"

#include "openswoole_timer_arginfo.h"

using openswoole::Timer;
using openswoole::TimerNode;
using zend::Function;

zend_class_entry *openswoole_timer_ce;
static zend_object_handlers openswoole_timer_handlers;

static zend_class_entry *openswoole_timer_iterator_ce;

OSW_EXTERN_C_BEGIN
PHP_FUNCTION(openswoole_timer_after);
PHP_FUNCTION(openswoole_timer_tick);
PHP_FUNCTION(openswoole_timer_exists);
PHP_FUNCTION(openswoole_timer_info);
PHP_FUNCTION(openswoole_timer_stats);
PHP_FUNCTION(openswoole_timer_list);
PHP_FUNCTION(openswoole_timer_clear);
PHP_FUNCTION(openswoole_timer_clear_all);
OSW_EXTERN_C_END

// clang-format off
static const zend_function_entry openswoole_timer_methods[] =
{
    ZEND_FENTRY(after, ZEND_FN(openswoole_timer_after), arginfo_class_OpenSwoole_Timer_after, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(tick, ZEND_FN(openswoole_timer_tick), arginfo_class_OpenSwoole_Timer_tick, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(exists, ZEND_FN(openswoole_timer_exists), arginfo_class_OpenSwoole_Timer_exists, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(info, ZEND_FN(openswoole_timer_info), arginfo_class_OpenSwoole_Timer_info, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(stats, ZEND_FN(openswoole_timer_stats), arginfo_class_OpenSwoole_Timer_stats, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(list, ZEND_FN(openswoole_timer_list), arginfo_class_OpenSwoole_Timer_list, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(clear, ZEND_FN(openswoole_timer_clear), arginfo_class_OpenSwoole_Timer_clear, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    ZEND_FENTRY(clearAll, ZEND_FN(openswoole_timer_clear_all), arginfo_class_OpenSwoole_Timer_clearAll, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
// clang-format on

void php_openswoole_timer_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY(openswoole_timer, "OpenSwoole\\Timer", "openswoole_timer", nullptr, openswoole_timer_methods);
    OSW_SET_CLASS_CREATE(openswoole_timer, osw_zend_create_object_deny);

    OSW_INIT_CLASS_ENTRY_BASE(openswoole_timer_iterator,
                             "OpenSwoole\\Timer\\Iterator",
                             "openswoole_timer_iterator",
                             nullptr,
                             nullptr,
                             spl_ce_ArrayIterator);

    zend_declare_class_constant_long(openswoole_timer_ce, ZEND_STRL("TIMER_MIN_MS"), OSW_TIMER_MIN_MS);
    zend_declare_class_constant_double(openswoole_timer_ce, ZEND_STRL("TIMER_MIN_SEC"), OSW_TIMER_MIN_SEC);
    zend_declare_class_constant_long(openswoole_timer_ce, ZEND_STRL("TIMER_MAX_MS"), OSW_TIMER_MAX_MS);
    zend_declare_class_constant_double(openswoole_timer_ce, ZEND_STRL("TIMER_MAX_SEC"), OSW_TIMER_MAX_SEC);

    // backward compatibility
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TIMER_MIN_MS", OSW_TIMER_MIN_MS);
    OSW_REGISTER_DOUBLE_CONSTANT("OPENSWOOLE_TIMER_MIN_SEC", OSW_TIMER_MIN_SEC);
    OSW_REGISTER_LONG_CONSTANT("OPENSWOOLE_TIMER_MAX_MS", OSW_TIMER_MAX_MS);
    OSW_REGISTER_DOUBLE_CONSTANT("OPENSWOOLE_TIMER_MAX_SEC", OSW_TIMER_MAX_SEC);
}

static void timer_dtor(TimerNode *tnode) {
    Function *fci = (Function *) tnode->data;
    osw_zend_fci_params_discard(&fci->fci);
    osw_zend_fci_cache_discard(&fci->fci_cache);
    efree(fci);
}

bool php_openswoole_timer_clear(TimerNode *tnode) {
    return openswoole_timer_del(tnode);
}

bool php_openswoole_timer_clear_all() {
    if (UNEXPECTED(!OpenSwooleTG.timer)) {
        return false;
    }

    size_t num = OpenSwooleTG.timer->count(), index = 0;
    TimerNode **list = (TimerNode **) emalloc(num * sizeof(TimerNode *));
    for (auto &kv : OpenSwooleTG.timer->get_map()) {
        TimerNode *tnode = kv.second;
        if (tnode->type == TimerNode::TYPE_PHP) {
            list[index++] = tnode;
        }
    }

    while (index--) {
        openswoole_timer_del(list[index]);
    }

    efree(list);

    return true;
}

static void timer_callback(Timer *timer, TimerNode *tnode) {
    Function *fci = (Function *) tnode->data;

    if (UNEXPECTED(!fci->call(nullptr, php_openswoole_is_enable_coroutine()))) {
        php_openswoole_error(E_WARNING, "%s->onTimeout handler error", ZSTR_VAL(openswoole_timer_ce->name));
    }
    if (!tnode->interval || tnode->removed) {
        timer_dtor(tnode);
    }
}

static void timer_add(INTERNAL_FUNCTION_PARAMETERS, bool persistent) {
    zend_long ms;
    Function *fci = (Function *) ecalloc(1, sizeof(Function));
    TimerNode *tnode;

    ZEND_PARSE_PARAMETERS_START(2, -1)
    Z_PARAM_LONG(ms)
    Z_PARAM_FUNC(fci->fci, fci->fci_cache)
    Z_PARAM_VARIADIC('*', fci->fci.params, fci->fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(goto _failed);

    if (UNEXPECTED(ms < OSW_TIMER_MIN_MS)) {
        php_openswoole_fatal_error(E_WARNING, "Timer must be greater than or equal to " ZEND_TOSTR(OSW_TIMER_MIN_MS));
    _failed:
        efree(fci);
        RETURN_FALSE;
    }

    // no server || user worker || task process with async mode
    if (!osw_server() || osw_server()->is_user_worker() ||
        (osw_server()->is_task_worker() && osw_server()->task_enable_coroutine)) {
        php_openswoole_check_reactor();
    }

    tnode = openswoole_timer_add(ms, persistent, timer_callback, fci);
    if (UNEXPECTED(!tnode)) {
        php_openswoole_fatal_error(E_WARNING, "add timer failed");
        goto _failed;
    }
    tnode->type = TimerNode::TYPE_PHP;
    tnode->destructor = timer_dtor;
    if (persistent) {
        if (fci->fci.param_count > 0) {
            uint32_t i;
            zval *params = (zval *) ecalloc(fci->fci.param_count + 1, sizeof(zval));
            for (i = 0; i < fci->fci.param_count; i++) {
                ZVAL_COPY(&params[i + 1], &fci->fci.params[i]);
            }
            fci->fci.params = params;
        } else {
            fci->fci.params = (zval *) emalloc(sizeof(zval));
        }
        fci->fci.param_count += 1;
        ZVAL_LONG(fci->fci.params, tnode->id);
    } else {
        osw_zend_fci_params_persist(&fci->fci);
    }
    osw_zend_fci_cache_persist(&fci->fci_cache);
    RETURN_LONG(tnode->id);
}

PHP_FUNCTION(openswoole_timer_after) {
    timer_add(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_FUNCTION(openswoole_timer_tick) {
    timer_add(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

PHP_FUNCTION(openswoole_timer_exists) {
    if (UNEXPECTED(!OpenSwooleTG.timer)) {
        RETURN_FALSE;
    } else {
        zend_long id;
        TimerNode *tnode;

        ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(id)
        ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

        tnode = openswoole_timer_get(id);
        RETURN_BOOL(tnode && !tnode->removed);
    }
}

PHP_FUNCTION(openswoole_timer_info) {
    if (UNEXPECTED(!OpenSwooleTG.timer)) {
        RETURN_FALSE;
    } else {
        zend_long id;
        TimerNode *tnode;

        ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(id)
        ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

        tnode = openswoole_timer_get(id);
        if (UNEXPECTED(!tnode)) {
            RETURN_FALSE;
        }
        array_init(return_value);
        add_assoc_long(return_value, "exec_msec", tnode->exec_msec);
        add_assoc_long(return_value, "interval", tnode->interval);
        add_assoc_long(return_value, "round", tnode->round);
        add_assoc_bool(return_value, "removed", tnode->removed);
    }
}

PHP_FUNCTION(openswoole_timer_stats) {
    array_init(return_value);
    if (OpenSwooleTG.timer) {
        add_assoc_bool(return_value, "initialized", 1);
        add_assoc_long(return_value, "num", OpenSwooleTG.timer->count());
        add_assoc_long(return_value, "round", OpenSwooleTG.timer->get_round());
    } else {
        add_assoc_bool(return_value, "initialized", 0);
        add_assoc_long(return_value, "num", 0);
        add_assoc_long(return_value, "round", 0);
    }
}

PHP_FUNCTION(openswoole_timer_list) {
    zval zlist;
    array_init(&zlist);
    if (EXPECTED(OpenSwooleTG.timer)) {
        for (auto &kv : OpenSwooleTG.timer->get_map()) {
            TimerNode *tnode = kv.second;
            if (tnode->type == TimerNode::TYPE_PHP) {
                add_next_index_long(&zlist, tnode->id);
            }
        }
    }
    object_init_ex(return_value, openswoole_timer_iterator_ce);
    osw_zend_call_method_with_1_params(
        return_value, openswoole_timer_iterator_ce, &openswoole_timer_iterator_ce->constructor, "__construct", nullptr, &zlist);
    zval_ptr_dtor(&zlist);
}

PHP_FUNCTION(openswoole_timer_clear) {
    if (UNEXPECTED(!OpenSwooleTG.timer)) {
        RETURN_FALSE;
    } else {
        zend_long id;

        ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(id)
        ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

        TimerNode *tnode = openswoole_timer_get(id);
        if (!tnode || tnode->type != TimerNode::TYPE_PHP) {
            RETURN_FALSE;
        }
        RETURN_BOOL(openswoole_timer_del(tnode));
    }
}

PHP_FUNCTION(openswoole_timer_clear_all) {
    RETURN_BOOL(php_openswoole_timer_clear_all());
}
