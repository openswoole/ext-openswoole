/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: e78ce13d50f0f6e47b7b0ea57132610ea15c80df */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Timer_after, 0, 2, MAY_BE_BOOL | MAY_BE_LONG)
ZEND_ARG_TYPE_INFO(0, ms, IS_LONG, 0)
ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_ARG_VARIADIC_TYPE_INFO(0, params, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Timer_tick arginfo_class_Swoole_Timer_after

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Timer_exists, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Timer_info, 0, 0, MAY_BE_ARRAY | MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Timer_stats, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Swoole_Timer_list, 0, 0, Swoole\\Timer\\Iterator, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Timer_clear arginfo_class_Swoole_Timer_exists

#define arginfo_class_Swoole_Timer_clearAll arginfo_class_Swoole_Timer_exists
