/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 73a61393cf18e5b785b93b5ecb6cbb9f55ccc4c5 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Lock___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_LONG, 0, "SWOOLE_MUTEX")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, lockFile, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Lock_lock, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Lock_lockwait, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Lock_trylock arginfo_class_Swoole_Lock_lock

#define arginfo_class_Swoole_Lock_lock_read arginfo_class_Swoole_Lock_lock

#define arginfo_class_Swoole_Lock_trylock_read arginfo_class_Swoole_Lock_lock

#define arginfo_class_Swoole_Lock_unlock arginfo_class_Swoole_Lock_lock

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Lock_destroy, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Lock___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()
