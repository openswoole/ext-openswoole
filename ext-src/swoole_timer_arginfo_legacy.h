
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Timer_after, 0, 0, 2)
ZEND_ARG_INFO(0, ms)
ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Timer_tick, 0, 0, 2)
ZEND_ARG_INFO(0, ms)
ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_ARG_VARIADIC_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Timer_exists, 0, 0, 1)
ZEND_ARG_INFO(0, timer_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Timer_info, 0, 0, 1)
ZEND_ARG_INFO(0, timer_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_timer_clear, 0, 0, 1)
ZEND_ARG_INFO(0, timer_id)
ZEND_END_ARG_INFO()
#define arginfo_class_Swoole_Timer_stats arginfo_swoole_void
#define arginfo_class_Swoole_Timer_list arginfo_swoole_void
#define arginfo_class_Swoole_Timer_clear arginfo_swoole_void
#define arginfo_class_Swoole_Timer_clearAll arginfo_swoole_void
