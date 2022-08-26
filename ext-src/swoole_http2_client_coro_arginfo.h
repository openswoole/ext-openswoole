/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 443a4017e69c3c097f5d7885254f130625e214a6 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "80")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, openSSL, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_set, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_connect, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http2_Client_stats, 0, 0, MAY_BE_BOOL|MAY_BE_ARRAY|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_isStreamExist, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, streamId, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http2_Client_send, 0, 1, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_OBJ_INFO(0, request, OpenSwoole\\Http2\\Request, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_write, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, streamId, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http2_Client_recv, 0, 0, OpenSwoole\\Http2\\Response, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http2_Client_read arginfo_class_Swoole_Coroutine_Http2_Client_recv

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_goaway, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, errorCode, IS_LONG, 0, "OpenSwoole\\Coroutine\\Http2\\Client::HTTP2_ERROR_NO_ERROR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, debugData, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http2_Client_ping arginfo_class_Swoole_Coroutine_Http2_Client_connect

#define arginfo_class_Swoole_Coroutine_Http2_Client_close arginfo_class_Swoole_Coroutine_Http2_Client_connect

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()
