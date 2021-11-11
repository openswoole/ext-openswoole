
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_Port_set, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, settings, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_Port_on, 0, 0, 2)
    ZEND_ARG_INFO(0, event_name)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Server_Port_getCallback, 0, 0, 1)
    ZEND_ARG_INFO(0, event_name)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Server_Port___construct arginfo_swoole_void
#define arginfo_class_Swoole_Server_Port___destruct arginfo_swoole_void
#define arginfo_class_Swoole_Server_Port_getSocket arginfo_swoole_void