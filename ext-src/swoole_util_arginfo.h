/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 0da42c2a97f76ce8e8e7d724b5b87300dd9407ec */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_getVersion, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_getCPUNum, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_getLocalIp, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_getLocalMac arginfo_class_Swoole_Util_getLocalIp

#define arginfo_class_Swoole_Util_getLastErrorCode arginfo_class_Swoole_Util_getCPUNum

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_getErrorMessage, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, errorCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_errorCode arginfo_class_Swoole_Util_getCPUNum

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_clearError, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_log, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Util_hashcode, 0, 0, MAY_BE_LONG|MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_mimeTypeAdd, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, suffix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, mimeType, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_mimeTypeSet arginfo_class_Swoole_Util_mimeTypeAdd

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_mimeTypeDel, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, suffix, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_mimeTypeGet, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Util_mimeTypeList arginfo_class_Swoole_Util_getLocalIp

#define arginfo_class_Swoole_Util_mimeTypeExists arginfo_class_Swoole_Util_mimeTypeGet

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Util_setProcessName, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()
