/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6736c80912bc4e84def1ec4e1a4c9cf5ec2f206a */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_swoole_table___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, table_size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, conflict_proportion, IS_DOUBLE, 1, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_column, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, type, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_create, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_swoole_table_destroy arginfo_class_swoole_table_create

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_set, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_swoole_table_get, 0, 2, MAY_BE_ARRAY|MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_del, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_swoole_table_exists arginfo_class_swoole_table_del

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_incr, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, incrby, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_decr, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, column, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, decrby, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_swoole_table_getSize arginfo_class_swoole_table_count

#define arginfo_class_swoole_table_getMemorySize arginfo_class_swoole_table_count

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_rewind, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_swoole_table_valid arginfo_class_swoole_table_create

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_next, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_swoole_table_current arginfo_class_swoole_table_next

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_swoole_table_key, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()
