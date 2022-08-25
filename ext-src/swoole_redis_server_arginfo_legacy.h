
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Redis_Server_addCommand, 0, 0, 2)
    ZEND_ARG_INFO(0, command)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Redis_Server_getCommand, 0, 0, 1)
    ZEND_ARG_INFO(0, command)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Redis_Server_format, 0, 0, 1)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
