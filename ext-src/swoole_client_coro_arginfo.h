/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 89b8afc8d475a3c273edf133492dc36314b6f382 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Client___construct, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, type, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, async, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO(0, id, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_set, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, settings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_connect, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0.5")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sock_flag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_send, 0, 1, MAY_BE_BOOL|MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_sendto, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_sendfile, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_recv, 0, 0, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_recvfrom, 0, 2, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(1, port, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_peek, 0, 0, MAY_BE_BOOL|MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "65535")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_close, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Client_isConnected arginfo_class_Swoole_Coroutine_Client_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_getsockname, 0, 0, MAY_BE_BOOL|MAY_BE_ARRAY)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Client_exportSocket, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Client_getpeername arginfo_class_Swoole_Coroutine_Client_getsockname

#define arginfo_class_Swoole_Coroutine_Client_enableSSL arginfo_class_Swoole_Coroutine_Client_close

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Client_getPeerCert, 0, 0, MAY_BE_BOOL|MAY_BE_STRING)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Client_verifyPeerCert arginfo_class_Swoole_Coroutine_Client_close

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Client___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()
