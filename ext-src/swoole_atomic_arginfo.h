/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f8c7aa4ac0892dc566fba6ef866cd88ff8703775 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Atomic___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Atomic_add, 0, 0, MAY_BE_LONG|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Atomic_sub arginfo_class_Swoole_Atomic_add

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Atomic_get, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Atomic_set, 0, 1, _IS_BOOL, 1)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Atomic_wait, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Atomic_wakeup, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Atomic_cmpset, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cmpVal, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, newVal, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Atomic_Long___construct arginfo_class_Swoole_Atomic___construct

#define arginfo_class_Swoole_Atomic_Long_add arginfo_class_Swoole_Atomic_add

#define arginfo_class_Swoole_Atomic_Long_sub arginfo_class_Swoole_Atomic_add

#define arginfo_class_Swoole_Atomic_Long_get arginfo_class_Swoole_Atomic_get

#define arginfo_class_Swoole_Atomic_Long_set arginfo_class_Swoole_Atomic_set

#define arginfo_class_Swoole_Atomic_Long_cmpset arginfo_class_Swoole_Atomic_cmpset
