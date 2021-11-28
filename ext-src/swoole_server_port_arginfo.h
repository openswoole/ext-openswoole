/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 74377646b1688d3c09e75f344e2b76152bf57d63 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_Port___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Server_Port_set, 0, 1, MAY_BE_BOOL|MAY_BE_VOID)
	ZEND_ARG_TYPE_INFO(0, settings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Server_Port_on, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Server_Port_getCallback, 0, 1, IS_CALLABLE, 1)
	ZEND_ARG_TYPE_INFO(0, event, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Server_Port_getSocket, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_Port___destruct arginfo_class_Swoole_Server_Port___construct
