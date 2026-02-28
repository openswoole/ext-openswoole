
ZEND_BEGIN_ARG_INFO_EX(arginfo_openswoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client___construct, 0, 0, 1)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, ssl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_set, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setDefer, 0, 0, 0)
ZEND_ARG_INFO(0, defer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setMethod, 0, 0, 1)
ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setHeaders, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, headers, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setBasicAuth, 0, 0, 2)
ZEND_ARG_INFO(0, username)
ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setCookies, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, cookies, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_setData, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_addFile, 0, 0, 2)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, type)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_addData, 0, 0, 2)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, type)
ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_execute, 0, 0, 1)
ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_get, 0, 0, 1)
ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_post, 0, 0, 2)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_download, 0, 0, 2)
ZEND_ARG_INFO(0, path)
ZEND_ARG_INFO(0, file)
ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_upgrade, 0, 0, 1)
ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_push, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, opcode)
ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_OpenSwoole_Coroutine_Http_Client_recv, 0, 0, 0)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

#define arginfo_class_OpenSwoole_Coroutine_Http_Client___destruct arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getDefer arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getpeername arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getsockname arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getBody arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getHeaders arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getCookies arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getStatusCode arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getHeaderOut arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_getPeerCert arginfo_openswoole_void
#define arginfo_class_OpenSwoole_Coroutine_Http_Client_close arginfo_openswoole_void
