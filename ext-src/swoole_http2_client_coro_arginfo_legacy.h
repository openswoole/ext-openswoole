
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, open_ssl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_stats, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_isStreamExist, 0, 0, 1)
    ZEND_ARG_INFO(0, stream_id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_send, 0, 0, 1)
    ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_write, 0, 0, 2)
    ZEND_ARG_INFO(0, stream_id)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, end_stream)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_recv, 0, 0, 0)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Http2_Client_goaway, 0, 0, 0)
    ZEND_ARG_INFO(0, error_code)
    ZEND_ARG_INFO(0, debug_data)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Http2_Client_connect arginfo_swoole_void
#define arginfo_class_Swoole_Coroutine_Http2_Client_read arginfo_class_Swoole_Coroutine_Http2_Client_recv
#define arginfo_class_Swoole_Coroutine_Http2_Client_ping arginfo_swoole_void
#define arginfo_class_Swoole_Coroutine_Http2_Client_close arginfo_swoole_void
#define arginfo_class_Swoole_Coroutine_Http2_Client___destruct arginfo_swoole_void
