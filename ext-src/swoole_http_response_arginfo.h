/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 59e390bde866e2f7674c7390714c6b73b6af6c1b */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_write, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_end, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, data, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_sendfile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, fileName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_redirect, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, status_code, IS_LONG, 0, "302")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_cookie, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, expire, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, path, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, domain, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, secure, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, httpOnly, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sameSite, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, priority, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Http_Response_rawcookie arginfo_class_Swoole_Http_Response_cookie

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_header, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, format, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_initHeader, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Http_Response_isWritable arginfo_class_Swoole_Http_Response_initHeader

#define arginfo_class_Swoole_Http_Response_detach arginfo_class_Swoole_Http_Response_initHeader

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Http_Response_create, 0, 0, Swoole\\Http\\Response, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, server, IS_MIXED, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fd, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Http_Response_upgrade arginfo_class_Swoole_Http_Response_initHeader

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_push, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_OBJ_TYPE_MASK(0, data, Swoole\\WebSocket\\Frame, MAY_BE_STRING, NULL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opcode, IS_LONG, 0, "SWOOLE_WEBSOCKET_OPCODE_TEXT")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "SWOOLE_WEBSOCKET_FLAG_FIN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Http_Response_recv, 0, 0, Swoole\\WebSocket\\Frame, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Http_Response_close arginfo_class_Swoole_Http_Response_initHeader

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_trailer, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Http_Response_ping arginfo_class_Swoole_Http_Response_initHeader

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_goaway, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, errorCode, IS_LONG, 0, "SWOOLE_HTTP2_ERROR_NO_ERROR")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, debugData, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Http_Response_status, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, statusCode, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, reason, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Http_Response___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()
