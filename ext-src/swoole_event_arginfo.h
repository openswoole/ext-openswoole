/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: a6e8df731a3c25928b93118e83acea6fd64cf22d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_add, 0, 4, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sock, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, read_callback, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO(0, write_callback, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Event_set arginfo_class_Swoole_Event_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_del, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sock, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_isset, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sock, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, flags, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_dispatch, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_defer, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_cycle, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, before, _IS_BOOL, 1, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_write, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sock, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Event_wait, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Event_rshutdown arginfo_class_Swoole_Event_wait

#define arginfo_class_Swoole_Event_exit arginfo_class_Swoole_Event_wait
