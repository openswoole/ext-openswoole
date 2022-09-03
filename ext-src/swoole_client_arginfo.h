/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 29a3009e0c5d56a4a48daea95fd8b124664800c0 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client___construct, 0, 0, 1)
ZEND_ARG_TYPE_INFO(0, sockType, IS_LONG, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, async, _IS_BOOL, 0, "false")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, id, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_set, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, settings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_connect, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0.5")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sockFlag, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Client_send, 0, 1, MAY_BE_BOOL | MAY_BE_LONG)
ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_sendto, 0, 3, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, ip, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_sendfile, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, fileName, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Client_recv, 0, 0, MAY_BE_BOOL | MAY_BE_STRING)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "65535")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_close, 0, 0, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, force, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_select, 0, 3, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(1, readReady, IS_ARRAY, 0)
ZEND_ARG_TYPE_INFO(1, writeReady, IS_ARRAY, 0)
ZEND_ARG_TYPE_INFO(1, errors, IS_ARRAY, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0.5")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_shutdown, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, how, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Client_isConnected, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Client_getsockname, 0, 0, MAY_BE_BOOL | MAY_BE_ARRAY)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Client_getSocket, 0, 0, Socket, MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Client_getpeername arginfo_class_Swoole_Client_getsockname

#define arginfo_class_Swoole_Client_enableSSL arginfo_class_Swoole_Client_isConnected

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Client_getPeerCert, 0, 0, MAY_BE_BOOL | MAY_BE_STRING)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Client_verifyPeerCert arginfo_class_Swoole_Client_isConnected

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()
