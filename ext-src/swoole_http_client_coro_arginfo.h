/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d1f65d69640b487c127ae6535bd6e2cc43339def */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client___construct, 0, 0, 2)
ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ssl, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_set, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, settings, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_getDefer, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setDefer, 0, 0, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defer, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setMethod, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setHeaders, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, headers, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setBasicAuth, 0, 2, _IS_BOOL, 1)
ZEND_ARG_TYPE_INFO(0, username, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, password, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setCookies, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, cookies, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_setData, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_MASK(0, data, MAY_BE_STRING | MAY_BE_ARRAY, NULL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_addFile, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mimeType, IS_STRING, 1, "null")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, fileName, IS_STRING, 0, "\"\"")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_addData, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mimeType, IS_STRING, 1, "null")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, filename, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_execute,
                                        0,
                                        1,
                                        MAY_BE_LONG | MAY_BE_BOOL)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_getpeername,
                                        0,
                                        0,
                                        MAY_BE_ARRAY | MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http_Client_getsockname arginfo_class_Swoole_Coroutine_Http_Client_getpeername

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_get, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_post, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_download, 0, 2, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_getBody,
                                        0,
                                        0,
                                        MAY_BE_STRING | MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_getHeaders,
                                        0,
                                        0,
                                        MAY_BE_ARRAY | MAY_BE_BOOL)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http_Client_getCookies arginfo_class_Swoole_Coroutine_Http_Client_getHeaders

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_getStatusCode,
                                        0,
                                        0,
                                        MAY_BE_LONG | MAY_BE_BOOL)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http_Client_getHeaderOut arginfo_class_Swoole_Coroutine_Http_Client_getBody

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Http_Client_getPeerCert,
                                        0,
                                        0,
                                        MAY_BE_STRING | MAY_BE_FALSE)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http_Client_upgrade arginfo_class_Swoole_Coroutine_Http_Client_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Http_Client_push, 0, 1, _IS_BOOL, 0)
ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opcode, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_OPCODE_TEXT")
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "OpenSwoole\\WebSocket\\Server::WEBSOCKET_FLAG_FIN")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(
    arginfo_class_Swoole_Coroutine_Http_Client_recv, 0, 0, OpenSwoole\\WebSocket\\Frame, MAY_BE_BOOL)
ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http_Client_close arginfo_class_Swoole_Coroutine_Http_Client_getDefer
