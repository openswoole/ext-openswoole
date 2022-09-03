
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_client_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client___construct, 0, 0, 1)
ZEND_ARG_INFO(0, type)
ZEND_ARG_INFO(0, async)
ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_set, 0, 0, 1)
ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_connect, 0, 0, 1)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, timeout)
ZEND_ARG_INFO(0, sock_flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_recv, 0, 0, 0)
ZEND_ARG_INFO(0, size)
ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_send, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, flag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_sendfile, 0, 0, 1)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, offset)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_sendto, 0, 0, 3)
ZEND_ARG_INFO(0, ip)
ZEND_ARG_INFO(0, port)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_close, 0, 0, 0)
ZEND_ARG_INFO(0, force)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_shutdown, 0, 0, 1)
ZEND_ARG_INFO(0, how)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Client_select, 0, 0, 3)
ZEND_ARG_INFO(1, read_array)
ZEND_ARG_INFO(1, write_array)
ZEND_ARG_INFO(1, error_array)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Client_isConnected arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_getsockname arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_getSocket arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_getpeername arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_enableSSL arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_getPeerCert arginfo_swoole_client_void
#define arginfo_class_Swoole_Client_verifyPeerCert arginfo_swoole_client_void
#define arginfo_class_Swoole_Client___destruct arginfo_swoole_client_void
