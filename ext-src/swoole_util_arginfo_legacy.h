ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_getVersion, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_getCPUNum, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_getLocalIp, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_getLocalMac arginfo_class_Swoole_Util_getLocalIp

#define arginfo_class_Swoole_Util_getLastErrorCode arginfo_class_Swoole_Util_getCPUNum

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_getErrorMessage, 0, 2, 0)
ZEND_ARG_INFO(0, errorCode)
ZEND_ARG_INFO(0, errorType)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_errorCode arginfo_class_Swoole_Util_getCPUNum

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_clearError, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_log, 0, 2, 0)
ZEND_ARG_INFO(0, level)
ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_hashcode, 0, 2, 0)
ZEND_ARG_INFO(0, content)
ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_mimeTypeAdd, 0, 2, 0)
ZEND_ARG_INFO(0, suffix)
ZEND_ARG_INFO(0, mimeType)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_mimeTypeSet arginfo_class_Swoole_Util_mimeTypeAdd

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_mimeTypeDel, 0, 1, 0)
ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_mimeTypeGet, 0, 1, 0)
ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_mimeTypeList arginfo_class_Swoole_Util_getLocalIp

#define arginfo_class_Swoole_Util_mimeTypeExists arginfo_class_Swoole_Util_mimeTypeGet

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_setProcessName, 0, 1, 0)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Util_setAio, 0, 1, 0)
ZEND_ARG_INFO(0, settings)
ZEND_END_ARG_INFO()
