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

#include "php_openswoole_private.h"
#include "openswoole_memory.h"

#include "openswoole_atomic_arginfo.h"

#ifdef HAVE_FUTEX
#include <linux/futex.h>
#include <syscall.h>

static osw_inline int openswoole_futex_wait(osw_atomic_t *atomic, double timeout) {
    if (osw_atomic_cmp_set(atomic, 1, 0)) {
        return OSW_OK;
    }

    int ret;
    struct timespec _timeout;

    if (timeout > 0) {
        _timeout.tv_sec = (long) timeout;
        _timeout.tv_nsec = (timeout - _timeout.tv_sec) * 1000 * 1000 * 1000;
        ret = syscall(SYS_futex, atomic, FUTEX_WAIT, 0, &_timeout, nullptr, 0);
    } else {
        ret = syscall(SYS_futex, atomic, FUTEX_WAIT, 0, nullptr, nullptr, 0);
    }
    if (ret == OSW_OK && osw_atomic_cmp_set(atomic, 1, 0)) {
        return OSW_OK;
    } else {
        return OSW_ERR;
    }
}

static osw_inline int openswoole_futex_wakeup(osw_atomic_t *atomic, int n) {
    if (osw_atomic_cmp_set(atomic, 0, 1)) {
        return syscall(SYS_futex, atomic, FUTEX_WAKE, n, nullptr, nullptr, 0);
    } else {
        return OSW_OK;
    }
}

#else
static osw_inline int openswoole_atomic_wait(osw_atomic_t *atomic, double timeout) {
    if (osw_atomic_cmp_set(atomic, (osw_atomic_t) 1, (osw_atomic_t) 0)) {
        return OSW_OK;
    }
    timeout = timeout <= 0 ? INT_MAX : timeout;
    int32_t i = (int32_t) osw_atomic_sub_fetch(atomic, 1);
    while (timeout > 0) {
        if ((int32_t) *atomic > i) {
            return OSW_OK;
        } else {
            usleep(1000);
            timeout -= 0.001;
        }
    }
    osw_atomic_fetch_add(atomic, 1);
    return OSW_ERR;
}

static osw_inline int openswoole_atomic_wakeup(osw_atomic_t *atomic, int n) {
    if (1 == (int32_t) *atomic) {
        return OSW_OK;
    }
    osw_atomic_fetch_add(atomic, n);
    return OSW_OK;
}
#endif

zend_class_entry *openswoole_atomic_ce;
static zend_object_handlers openswoole_atomic_handlers;

zend_class_entry *openswoole_atomic_long_ce;
static zend_object_handlers openswoole_atomic_long_handlers;

struct AtomicObject {
    osw_atomic_t *ptr;
    zend_object std;
};

static osw_inline AtomicObject *php_openswoole_atomic_fetch_object(zend_object *obj) {
    return (AtomicObject *) ((char *) obj - openswoole_atomic_handlers.offset);
}

static osw_atomic_t *php_openswoole_atomic_get_ptr(zval *zobject) {
    return php_openswoole_atomic_fetch_object(Z_OBJ_P(zobject))->ptr;
}

void php_openswoole_atomic_set_ptr(zval *zobject, osw_atomic_t *ptr) {
    php_openswoole_atomic_fetch_object(Z_OBJ_P(zobject))->ptr = ptr;
}

static void php_openswoole_atomic_free_object(zend_object *object) {
    osw_mem_pool()->free((void *) php_openswoole_atomic_fetch_object(object)->ptr);
    zend_object_std_dtor(object);
}

static zend_object *php_openswoole_atomic_create_object(zend_class_entry *ce) {
    AtomicObject *atomic = (AtomicObject *) zend_object_alloc(sizeof(AtomicObject), ce);
    if (atomic == nullptr) {
        zend_throw_exception(openswoole_exception_ce, "global memory allocation failure", OSW_ERROR_MALLOC_FAIL);
    }

    zend_object_std_init(&atomic->std, ce);
    object_properties_init(&atomic->std, ce);
    atomic->std.handlers = &openswoole_atomic_handlers;
    atomic->ptr = (osw_atomic_t *) osw_mem_pool()->alloc(sizeof(osw_atomic_t));
    if (atomic->ptr == nullptr) {
        zend_throw_exception(openswoole_exception_ce, "global memory allocation failure", OSW_ERROR_MALLOC_FAIL);
    }

    return &atomic->std;
}

struct AtomicLongObject {
    osw_atomic_long_t *ptr;
    zend_object std;
};

static osw_inline AtomicLongObject *php_openswoole_atomic_long_fetch_object(zend_object *obj) {
    return (AtomicLongObject *) ((char *) obj - openswoole_atomic_long_handlers.offset);
}

static osw_atomic_long_t *php_openswoole_atomic_long_get_ptr(zval *zobject) {
    return php_openswoole_atomic_long_fetch_object(Z_OBJ_P(zobject))->ptr;
}

void php_openswoole_atomic_long_set_ptr(zval *zobject, osw_atomic_long_t *ptr) {
    php_openswoole_atomic_long_fetch_object(Z_OBJ_P(zobject))->ptr = ptr;
}

static void php_openswoole_atomic_long_free_object(zend_object *object) {
    osw_mem_pool()->free((void *) php_openswoole_atomic_long_fetch_object(object)->ptr);
    zend_object_std_dtor(object);
}

static zend_object *php_openswoole_atomic_long_create_object(zend_class_entry *ce) {
    AtomicLongObject *atomic_long = (AtomicLongObject *) zend_object_alloc(sizeof(AtomicLongObject), ce);
    if (atomic_long == nullptr) {
        zend_throw_exception(openswoole_exception_ce, "global memory allocation failure", OSW_ERROR_MALLOC_FAIL);
    }

    zend_object_std_init(&atomic_long->std, ce);
    object_properties_init(&atomic_long->std, ce);
    atomic_long->std.handlers = &openswoole_atomic_long_handlers;

    atomic_long->ptr = (osw_atomic_long_t *) osw_mem_pool()->alloc(sizeof(osw_atomic_long_t));
    if (atomic_long->ptr == nullptr) {
        zend_throw_exception(openswoole_exception_ce, "global memory allocation failure", OSW_ERROR_MALLOC_FAIL);
    }

    return &atomic_long->std;
}

OSW_EXTERN_C_BEGIN
static PHP_METHOD(openswoole_atomic, __construct);
static PHP_METHOD(openswoole_atomic, add);
static PHP_METHOD(openswoole_atomic, sub);
static PHP_METHOD(openswoole_atomic, get);
static PHP_METHOD(openswoole_atomic, set);
static PHP_METHOD(openswoole_atomic, cmpset);
static PHP_METHOD(openswoole_atomic, wait);
static PHP_METHOD(openswoole_atomic, wakeup);

static PHP_METHOD(openswoole_atomic_long, __construct);
static PHP_METHOD(openswoole_atomic_long, add);
static PHP_METHOD(openswoole_atomic_long, sub);
static PHP_METHOD(openswoole_atomic_long, get);
static PHP_METHOD(openswoole_atomic_long, set);
static PHP_METHOD(openswoole_atomic_long, cmpset);
OSW_EXTERN_C_END

// clang-format off

static const zend_function_entry openswoole_atomic_methods[] =
{
    PHP_ME(openswoole_atomic, __construct, arginfo_class_OpenSwoole_Atomic___construct, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, add, arginfo_class_OpenSwoole_Atomic_add, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, sub, arginfo_class_OpenSwoole_Atomic_sub, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, get, arginfo_class_OpenSwoole_Atomic_get, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, set, arginfo_class_OpenSwoole_Atomic_set, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, wait, arginfo_class_OpenSwoole_Atomic_wait, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, wakeup, arginfo_class_OpenSwoole_Atomic_wakeup, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic, cmpset, arginfo_class_OpenSwoole_Atomic_cmpset, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

static const zend_function_entry openswoole_atomic_long_methods[] =
{
    PHP_ME(openswoole_atomic_long, __construct, arginfo_class_OpenSwoole_Atomic___construct, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic_long, add, arginfo_class_OpenSwoole_Atomic_add, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic_long, sub, arginfo_class_OpenSwoole_Atomic_sub, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic_long, get, arginfo_class_OpenSwoole_Atomic_get, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic_long, set, arginfo_class_OpenSwoole_Atomic_set, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_atomic_long, cmpset, arginfo_class_OpenSwoole_Atomic_cmpset, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

// clang-format on

void php_openswoole_atomic_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY(openswoole_atomic, "OpenSwoole\\Atomic", "openswoole_atomic", nullptr, openswoole_atomic_methods);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_atomic);
    OSW_SET_CLASS_CLONEABLE(openswoole_atomic, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_atomic, osw_zend_class_unset_property_deny);
    OSW_SET_CLASS_CUSTOM_OBJECT(
        openswoole_atomic, php_openswoole_atomic_create_object, php_openswoole_atomic_free_object, AtomicObject, std);

    OSW_INIT_CLASS_ENTRY(
        openswoole_atomic_long, "OpenSwoole\\Atomic\\Long", "openswoole_atomic_long", nullptr, openswoole_atomic_long_methods);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_atomic_long);
    OSW_SET_CLASS_CLONEABLE(openswoole_atomic_long, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_atomic_long, osw_zend_class_unset_property_deny);
    OSW_SET_CLASS_CUSTOM_OBJECT(openswoole_atomic_long,
                               php_openswoole_atomic_long_create_object,
                               php_openswoole_atomic_long_free_object,
                               AtomicLongObject,
                               std);
}

PHP_METHOD(openswoole_atomic, __construct) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long value = 0;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    *atomic = (osw_atomic_t) value;
}

PHP_METHOD(openswoole_atomic, add) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long value = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_LONG(osw_atomic_add_fetch(atomic, (uint32_t) value));
}

PHP_METHOD(openswoole_atomic, sub) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long value = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_LONG(osw_atomic_sub_fetch(atomic, (uint32_t) value));
}

PHP_METHOD(openswoole_atomic, get) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    RETURN_LONG(*atomic);
}

PHP_METHOD(openswoole_atomic, set) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    *atomic = (uint32_t) value;
}

PHP_METHOD(openswoole_atomic, cmpset) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long cmp_val, new_val;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(cmp_val)
    Z_PARAM_LONG(new_val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(osw_atomic_cmp_set(atomic, (osw_atomic_t) cmp_val, (osw_atomic_t) new_val));
}

PHP_METHOD(openswoole_atomic, wait) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    double timeout = 1.0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#ifdef HAVE_FUTEX
    OSW_CHECK_RETURN(openswoole_futex_wait(atomic, timeout));
#else
    OSW_CHECK_RETURN(openswoole_atomic_wait(atomic, timeout));
#endif
}

PHP_METHOD(openswoole_atomic, wakeup) {
    osw_atomic_t *atomic = php_openswoole_atomic_get_ptr(ZEND_THIS);
    zend_long n = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(n)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

#ifdef HAVE_FUTEX
    OSW_CHECK_RETURN(openswoole_futex_wakeup(atomic, (int) n));
#else
    OSW_CHECK_RETURN(openswoole_atomic_wakeup(atomic, n));
#endif
}

PHP_METHOD(openswoole_atomic_long, __construct) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    zend_long value = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    *atomic_long = (osw_atomic_long_t) value;
    RETURN_TRUE;
}

PHP_METHOD(openswoole_atomic_long, add) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    zend_long value = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_LONG(osw_atomic_add_fetch(atomic_long, (osw_atomic_long_t) value));
}

PHP_METHOD(openswoole_atomic_long, sub) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    zend_long value = 1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_LONG(osw_atomic_sub_fetch(atomic_long, (osw_atomic_long_t) value));
}

PHP_METHOD(openswoole_atomic_long, get) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    RETURN_LONG(*atomic_long);
}

PHP_METHOD(openswoole_atomic_long, set) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    zend_long value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    *atomic_long = (osw_atomic_long_t) value;
}

PHP_METHOD(openswoole_atomic_long, cmpset) {
    osw_atomic_long_t *atomic_long = php_openswoole_atomic_long_get_ptr(ZEND_THIS);
    zend_long cmp_val, set_val;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(cmp_val)
    Z_PARAM_LONG(set_val)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(osw_atomic_cmp_set(atomic_long, (osw_atomic_long_t) cmp_val, (osw_atomic_long_t) set_val));
}
