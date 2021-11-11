
ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_table_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table___construct, 0, 0, 1)
    ZEND_ARG_INFO(0, table_size)
    ZEND_ARG_INFO(0, conflict_proportion)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_column, 0, 0, 2)
    ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_set, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_ARRAY_INFO(0, value, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_get, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, field)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_exists, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_del, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_incr, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, column)
    ZEND_ARG_INFO(0, incrby)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Table_decr, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, column)
    ZEND_ARG_INFO(0, decrby)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Table_create arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_destroy arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_getSize arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_getMemorySize arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_valid arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_current arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_count arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_rewind arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_next arginfo_swoole_table_void
#define arginfo_class_Swoole_Table_key arginfo_swoole_table_void
