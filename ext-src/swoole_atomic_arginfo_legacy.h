
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_add, 0, 0, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_sub, 0, 0, 0)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_get, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_set, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_cmpset, 0, 0, 2)
    ZEND_ARG_INFO(0, cmp_val)
    ZEND_ARG_INFO(0, new_val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_wait, 0, 0, 0)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic_wakeup, 0, 0, 0)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()
