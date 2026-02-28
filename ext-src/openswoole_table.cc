/*
  +----------------------------------------------------------------------+
  | OpenSwoole                                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | hello@swoole.co.uk so we can mail you a copy immediately.            |
  +----------------------------------------------------------------------+
  | Author: Tianfeng Han  <mikan.tenny@gmail.com>                        |
  +----------------------------------------------------------------------+
*/

#include "php_openswoole_cxx.h"

#include "openswoole_table.h"
#include "zend_exceptions.h"

#include "openswoole_table_arginfo.h"

using namespace openswoole;

static inline void php_openswoole_table_row2array(Table *table, TableRow *row, zval *return_value) {
    array_init(return_value);

    for (auto i = table->column_list->begin(); i != table->column_list->end(); i++) {
        TableColumn *col = *i;
        if (col->type == TableColumn::TYPE_STRING) {
            TableStringLength len = 0;
            char *str = nullptr;
            row->get_value(col, &str, &len);
            add_assoc_stringl_ex(return_value, col->name.c_str(), col->name.length(), str, len);
        } else if (col->type == TableColumn::TYPE_FLOAT) {
            double dval = 0;
            row->get_value(col, &dval);
            add_assoc_double_ex(return_value, col->name.c_str(), col->name.length(), dval);
        } else if (col->type == TableColumn::TYPE_INT) {
            long lval = 0;
            row->get_value(col, &lval);
            add_assoc_long_ex(return_value, col->name.c_str(), col->name.length(), lval);
        } else {
            abort();
        }
    }
}

static inline void php_openswoole_table_get_field_value(
    Table *table, TableRow *row, zval *return_value, char *field, uint16_t field_len) {
    TableColumn *col = table->get_column(std::string(field, field_len));
    if (!col) {
        ZVAL_FALSE(return_value);
        return;
    }
    if (col->type == TableColumn::TYPE_STRING) {
        TableStringLength len = 0;
        char *str = nullptr;
        row->get_value(col, &str, &len);
        ZVAL_STRINGL(return_value, str, len);
    } else if (col->type == TableColumn::TYPE_FLOAT) {
        double dval = 0;
        row->get_value(col, &dval);
        ZVAL_DOUBLE(return_value, dval);
    } else if (col->type == TableColumn::TYPE_INT) {
        long lval = 0;
        row->get_value(col, &lval);
        ZVAL_LONG(return_value, lval);
    } else {
        abort();
    }
}

static zend_class_entry *openswoole_table_ce;
static zend_object_handlers openswoole_table_handlers;

struct TableObject {
    Table *ptr;
    zend_object std;
};

static inline TableObject *php_openswoole_table_fetch_object(zend_object *obj) {
    return (TableObject *) ((char *) obj - openswoole_table_handlers.offset);
}

static inline Table *php_openswoole_table_get_ptr(zval *zobject) {
    return php_openswoole_table_fetch_object(Z_OBJ_P(zobject))->ptr;
}

static inline Table *php_openswoole_table_get_and_check_ptr(zval *zobject) {
    Table *table = php_openswoole_table_get_ptr(zobject);
    if (!table) {
        php_openswoole_fatal_error(E_ERROR, "you must call Table constructor first");
    }
    return table;
}

static inline Table *php_openswoole_table_get_and_check_ptr2(zval *zobject) {
    Table *table = php_openswoole_table_get_and_check_ptr(zobject);
    if (!table->ready()) {
        php_openswoole_fatal_error(E_ERROR, "table is not created or has been destroyed");
    }
    return table;
}

static void inline php_openswoole_table_set_ptr(zval *zobject, Table *ptr) {
    php_openswoole_table_fetch_object(Z_OBJ_P(zobject))->ptr = ptr;
}

static inline void php_openswoole_table_free_object(zend_object *object) {
    Table *table = php_openswoole_table_fetch_object(object)->ptr;
    if (table) {
        table->free();
    }
    zend_object_std_dtor(object);
}

static inline zend_object *php_openswoole_table_create_object(zend_class_entry *ce) {
    TableObject *table = (TableObject *) zend_object_alloc(sizeof(TableObject), ce);
    zend_object_std_init(&table->std, ce);
    object_properties_init(&table->std, ce);
    table->std.handlers = &openswoole_table_handlers;
    return &table->std;
}

OSW_EXTERN_C_BEGIN
static PHP_METHOD(openswoole_table, __construct);
static PHP_METHOD(openswoole_table, column);
static PHP_METHOD(openswoole_table, create);
static PHP_METHOD(openswoole_table, set);
static PHP_METHOD(openswoole_table, get);
static PHP_METHOD(openswoole_table, del);
static PHP_METHOD(openswoole_table, exists);
static PHP_METHOD(openswoole_table, incr);
static PHP_METHOD(openswoole_table, decr);
static PHP_METHOD(openswoole_table, count);
static PHP_METHOD(openswoole_table, destroy);
static PHP_METHOD(openswoole_table, getSize);
static PHP_METHOD(openswoole_table, getMemorySize);

static PHP_METHOD(openswoole_table, rewind);
static PHP_METHOD(openswoole_table, next);
static PHP_METHOD(openswoole_table, current);
static PHP_METHOD(openswoole_table, key);
static PHP_METHOD(openswoole_table, valid);

OSW_EXTERN_C_END

// clang-format off
static const zend_function_entry openswoole_table_methods[] =
{
    PHP_ME(openswoole_table, __construct, arginfo_class_OpenSwoole_Table___construct, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, column,      arginfo_class_OpenSwoole_Table_column, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, create,      arginfo_class_OpenSwoole_Table_create, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, destroy,     arginfo_class_OpenSwoole_Table_destroy, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, set,         arginfo_class_OpenSwoole_Table_set, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, get,         arginfo_class_OpenSwoole_Table_get, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, count,       arginfo_class_OpenSwoole_Table_count, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, del,         arginfo_class_OpenSwoole_Table_del, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, exists,      arginfo_class_OpenSwoole_Table_exists, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, incr,        arginfo_class_OpenSwoole_Table_incr, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, decr,        arginfo_class_OpenSwoole_Table_decr, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, getSize,    arginfo_class_OpenSwoole_Table_getSize, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, getMemorySize,    arginfo_class_OpenSwoole_Table_getMemorySize, ZEND_ACC_PUBLIC)
    // implement Iterator
    PHP_ME(openswoole_table, rewind,      arginfo_class_OpenSwoole_Table_rewind, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, valid,       arginfo_class_OpenSwoole_Table_valid, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, next,        arginfo_class_OpenSwoole_Table_next, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, current,     arginfo_class_OpenSwoole_Table_current, ZEND_ACC_PUBLIC)
    PHP_ME(openswoole_table, key,         arginfo_class_OpenSwoole_Table_key, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
// clang-format on

void php_openswoole_table_minit(int module_number) {
    OSW_INIT_CLASS_ENTRY(openswoole_table, "OpenSwoole\\Table", "openswoole_table", nullptr, openswoole_table_methods);
    OSW_SET_CLASS_NOT_SERIALIZABLE(openswoole_table);
    OSW_SET_CLASS_CLONEABLE(openswoole_table, osw_zend_class_clone_deny);
    OSW_SET_CLASS_UNSET_PROPERTY_HANDLER(openswoole_table, osw_zend_class_unset_property_deny);
    OSW_SET_CLASS_CUSTOM_OBJECT(
        openswoole_table, php_openswoole_table_create_object, php_openswoole_table_free_object, TableObject, std);
    zend_class_implements(openswoole_table_ce, 1, zend_ce_iterator);
#ifdef OSW_HAVE_COUNTABLE
    zend_class_implements(openswoole_table_ce, 1, zend_ce_countable);
#endif

    zend_declare_property_null(openswoole_table_ce, ZEND_STRL("size"), ZEND_ACC_PUBLIC);
    zend_declare_property_null(openswoole_table_ce, ZEND_STRL("memorySize"), ZEND_ACC_PUBLIC);

    zend_declare_class_constant_long(openswoole_table_ce, ZEND_STRL("TYPE_INT"), TableColumn::TYPE_INT);
    zend_declare_class_constant_long(openswoole_table_ce, ZEND_STRL("TYPE_STRING"), TableColumn::TYPE_STRING);
    zend_declare_class_constant_long(openswoole_table_ce, ZEND_STRL("TYPE_FLOAT"), TableColumn::TYPE_FLOAT);
}

PHP_METHOD(openswoole_table, __construct) {
    Table *table = php_openswoole_table_get_ptr(ZEND_THIS);
    if (table) {
        php_openswoole_fatal_error(E_ERROR, "Constructor of %s can only be called once", OSW_Z_OBJCE_NAME_VAL_P(ZEND_THIS));
    }

    zend_long table_size;
    double conflict_proportion = OSW_TABLE_CONFLICT_PROPORTION;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
    Z_PARAM_LONG(table_size)
    Z_PARAM_OPTIONAL
    Z_PARAM_DOUBLE(conflict_proportion)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    table = Table::make(table_size, conflict_proportion);
    if (table == nullptr) {
        zend_throw_exception(openswoole_exception_ce, "global memory allocation failure", OSW_ERROR_MALLOC_FAIL);
        RETURN_FALSE;
    }
    table->set_hash_func([](const char *key, size_t len) -> uint64_t {
        return zend_string_hash_val(zend::fetch_zend_string_by_val((void *) key));
    });
    php_openswoole_table_set_ptr(ZEND_THIS, table);
}

PHP_METHOD(openswoole_table, column) {
    Table *table = php_openswoole_table_get_and_check_ptr(ZEND_THIS);
    char *name;
    size_t len;
    long type;
    long size = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sl|l", &name, &len, &type, &size) == FAILURE) {
        RETURN_FALSE;
    }
    if (type == TableColumn::TYPE_STRING) {
        if (size < 1) {
            php_openswoole_fatal_error(E_WARNING, "the length of string type values has to be more than zero");
            RETURN_FALSE;
        }
        size = OSW_MEM_ALIGNED_SIZE(size);
    }
    if (table->ready()) {
        php_openswoole_fatal_error(E_WARNING, "unable to add column after table has been created");
        RETURN_FALSE;
    }
    RETURN_BOOL(table->add_column(std::string(name, len), (enum TableColumn::Type) type, size));
}

static PHP_METHOD(openswoole_table, create) {
    Table *table = php_openswoole_table_get_and_check_ptr(ZEND_THIS);

    if (!table->create()) {
        php_openswoole_fatal_error(E_ERROR, "unable to allocate memory");
        RETURN_FALSE;
    }
    zend_update_property_long(openswoole_table_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("size"), table->get_size());
    zend_update_property_long(
        openswoole_table_ce, OSW_Z8_OBJ_P(ZEND_THIS), ZEND_STRL("memorySize"), table->get_memory_size());
    RETURN_TRUE;
}

static PHP_METHOD(openswoole_table, destroy) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);

    table->destroy();
    php_openswoole_table_set_ptr(ZEND_THIS, nullptr);
    RETURN_TRUE;
}

static PHP_METHOD(openswoole_table, set) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    zval *array;
    char *key;
    size_t keylen;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 2, 2)
    Z_PARAM_STRING(key, keylen)
    Z_PARAM_ARRAY(array)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (!table->ready()) {
        php_openswoole_fatal_error(E_ERROR, "the table object does not exist");
        RETURN_FALSE;
    }

    if (keylen >= OSW_TABLE_KEY_SIZE) {
        php_openswoole_fatal_error(E_WARNING, "key[%s] is too long", key);
    }

    int out_flags;
    TableRow *_rowlock = nullptr;
    TableRow *row = table->set(key, keylen, &_rowlock, &out_flags);
    if (!row) {
        _rowlock->unlock();
        zend_throw_exception(
            openswoole_exception_ce, "failed to set key value, try to increase the table_size", OSW_ERROR_MALLOC_FAIL);
        RETURN_FALSE;
    }

    HashTable *ht = Z_ARRVAL_P(array);

    if (out_flags & OSW_TABLE_FLAG_NEW_ROW) {
        for (auto i = table->column_list->begin(); i != table->column_list->end(); i++) {
            TableColumn *col = *i;
            zval *zv = zend_hash_str_find(ht, col->name.c_str(), col->name.length());
            if (zv == nullptr || ZVAL_IS_NULL(zv)) {
                col->clear(row);
            } else {
                if (col->type == TableColumn::TYPE_STRING) {
                    if (Z_TYPE_P(zv) != IS_STRING) {
                        zend_throw_exception_ex(openswoole_exception_ce,
                                                -1,
                                                "[key=%s,field=%s] type error for TYPE_STRING column",
                                                key,
                                                col->name.c_str());
                    }
                    zend_string *str = zval_get_string(zv);
                    if (ZSTR_LEN(str) > col->size - sizeof(TableStringLength)) {
                        zend_throw_exception_ex(openswoole_exception_ce,
                                                -1,
                                                "[key=%s,field=%s] value is too long: %zu, maximum length: %lu",
                                                key,
                                                col->name.c_str(),
                                                ZSTR_LEN(str),
                                                col->size - sizeof(TableStringLength));
                    }
                    row->set_value(col, ZSTR_VAL(str), ZSTR_LEN(str));
                    zend_string_release(str);
                } else if (col->type == TableColumn::TYPE_FLOAT) {
                    if (Z_TYPE_P(zv) != IS_DOUBLE) {
                        zend_throw_exception_ex(openswoole_exception_ce,
                                                -1,
                                                "[key=%s,field=%s] type error for TYPE_FLOAT column",
                                                key,
                                                col->name.c_str());
                    }
                    double _value = zval_get_double(zv);
                    row->set_value(col, &_value, 0);
                } else {
                    if (Z_TYPE_P(zv) != IS_LONG) {
                        zend_throw_exception_ex(openswoole_exception_ce,
                                                -1,
                                                "[key=%s,field=%s] type error for TYPE_INT column",
                                                key,
                                                col->name.c_str());
                    }
                    long _value = zval_get_long(zv);
                    row->set_value(col, &_value, 0);
                }
            }
        }
    } else {
        const char *k;
        uint32_t klen;
        int ktype;
        zval *zv;
        OSW_HASHTABLE_FOREACH_START2(ht, k, klen, ktype, zv) {
            if (k == nullptr) {
                continue;
            }
            TableColumn *col = table->get_column(std::string(k, klen));
            if (col == nullptr) {
                continue;
            } else if (col->type == TableColumn::TYPE_STRING) {
                if (Z_TYPE_P(zv) != IS_STRING) {
                    zend_throw_exception_ex(openswoole_exception_ce,
                                            -1,
                                            "[key=%s,field=%s] type error for TYPE_STRING column",
                                            key,
                                            col->name.c_str());
                }
                zend_string *str = zval_get_string(zv);
                if (ZSTR_LEN(str) > col->size - sizeof(TableStringLength)) {
                    zend_throw_exception_ex(openswoole_exception_ce,
                                            -1,
                                            "[key=%s,field=%s] value is too long: %zu, maximum length: %lu",
                                            key,
                                            col->name.c_str(),
                                            ZSTR_LEN(str),
                                            col->size - sizeof(TableStringLength));
                }
                row->set_value(col, ZSTR_VAL(str), ZSTR_LEN(str));
                zend_string_release(str);
            } else if (col->type == TableColumn::TYPE_FLOAT) {
                if (Z_TYPE_P(zv) != IS_DOUBLE) {
                    zend_throw_exception_ex(openswoole_exception_ce,
                                            -1,
                                            "[key=%s,field=%s] type error for TYPE_FLOAT column",
                                            key,
                                            col->name.c_str());
                }
                double _value = zval_get_double(zv);
                row->set_value(col, &_value, 0);
            } else {
                if (Z_TYPE_P(zv) != IS_LONG) {
                    zend_throw_exception_ex(openswoole_exception_ce,
                                            -1,
                                            "[key=%s,field=%s] type error for TYPE_INT column",
                                            key,
                                            col->name.c_str());
                }
                long _value = zval_get_long(zv);
                row->set_value(col, &_value, 0);
            }
        }
        (void) ktype;
        OSW_HASHTABLE_FOREACH_END();
    }
    _rowlock->unlock();
    RETURN_TRUE;
}

static PHP_METHOD(openswoole_table, incr) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    char *key;
    size_t key_len;
    char *col;
    size_t col_len;
    zval *incrby = nullptr;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|z", &key, &key_len, &col, &col_len, &incrby) == FAILURE) {
        RETURN_FALSE;
    }

    int out_flags;
    TableRow *_rowlock = nullptr;
    TableRow *row = table->set(key, key_len, &_rowlock, &out_flags);
    if (!row) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "unable to allocate memory");
        RETURN_FALSE;
    }

    TableColumn *column = table->get_column(std::string(col, col_len));
    if (column == nullptr) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "column[%s] does not exist", col);
        RETURN_FALSE;
    }

    if (out_flags & OSW_TABLE_FLAG_NEW_ROW) {
        table->clear_row(row);
    }

    if (column->type == TableColumn::TYPE_STRING) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "can't execute 'incr' on a string type column");
        RETURN_FALSE;
    } else if (column->type == TableColumn::TYPE_FLOAT) {
        double set_value = 0;
        memcpy(&set_value, row->data + column->index, sizeof(set_value));
        if (incrby) {
            set_value += zval_get_double(incrby);
        } else {
            set_value += 1;
        }
        row->set_value(column, &set_value, 0);
        RETVAL_DOUBLE(set_value);
    } else {
        long set_value = 0;
        memcpy(&set_value, row->data + column->index, sizeof(set_value));
        if (incrby) {
            set_value += zval_get_long(incrby);
        } else {
            set_value += 1;
        }
        row->set_value(column, &set_value, 0);
        RETVAL_LONG(set_value);
    }
    _rowlock->unlock();
}

static PHP_METHOD(openswoole_table, decr) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    char *key;
    size_t key_len;
    char *col;
    size_t col_len;
    zval *decrby = nullptr;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss|z", &key, &key_len, &col, &col_len, &decrby) == FAILURE) {
        RETURN_FALSE;
    }

    int out_flags;
    TableRow *_rowlock = nullptr;
    TableRow *row = table->set(key, key_len, &_rowlock, &out_flags);
    if (!row) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "unable to allocate memory");
        RETURN_FALSE;
    }

    TableColumn *column = table->get_column(std::string(col, col_len));
    if (column == nullptr) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "column[%s] does not exist", col);
        RETURN_FALSE;
    }

    if (out_flags & OSW_TABLE_FLAG_NEW_ROW) {
        table->clear_row(row);
    }

    if (column->type == TableColumn::TYPE_STRING) {
        _rowlock->unlock();
        php_openswoole_fatal_error(E_WARNING, "can't execute 'decr' on a string type column");
        RETURN_FALSE;
    } else if (column->type == TableColumn::TYPE_FLOAT) {
        double set_value = 0;
        memcpy(&set_value, row->data + column->index, sizeof(set_value));
        if (decrby) {
            set_value -= zval_get_double(decrby);
        } else {
            set_value -= 1;
        }
        row->set_value(column, &set_value, 0);
        RETVAL_DOUBLE(set_value);
    } else {
        long set_value = 0;
        memcpy(&set_value, row->data + column->index, sizeof(set_value));
        if (decrby) {
            set_value -= zval_get_long(decrby);
        } else {
            set_value -= 1;
        }
        row->set_value(column, &set_value, 0);
        RETVAL_LONG(set_value);
    }
    _rowlock->unlock();
}

static PHP_METHOD(openswoole_table, get) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    char *key;
    size_t keylen;
    char *column = nullptr;
    size_t column_len = 0;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 2)
    Z_PARAM_STRING(key, keylen)
    Z_PARAM_OPTIONAL
    Z_PARAM_STRING(column, column_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    TableRow *_rowlock = nullptr;
    TableRow *row = table->get(key, keylen, &_rowlock);
    if (!row) {
        RETVAL_FALSE;
    } else if (column && column_len > 0) {
        php_openswoole_table_get_field_value(table, row, return_value, column, (uint16_t) column_len);
    } else {
        php_openswoole_table_row2array(table, row, return_value);
    }
    _rowlock->unlock();
}

static PHP_METHOD(openswoole_table, exists) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    char *key;
    size_t keylen;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &key, &keylen) == FAILURE) {
        RETURN_FALSE;
    }
    RETURN_BOOL(table->exists(key, keylen));
}

static PHP_METHOD(openswoole_table, del) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    char *key;
    size_t keylen;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
    Z_PARAM_STRING(key, keylen)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    RETURN_BOOL(table->del(key, keylen));
}

static PHP_METHOD(openswoole_table, count) {
#define COUNT_NORMAL 0
#define COUNT_RECURSIVE 1
    Table *table = php_openswoole_table_get_ptr(ZEND_THIS);
    if (!table) {
        RETURN_LONG(0);
    }

    zend_long mode = COUNT_NORMAL;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &mode) == FAILURE) {
        RETURN_FALSE;
    }

    if (mode == COUNT_NORMAL) {
        RETURN_LONG(table->count());
    } else {
        RETURN_LONG(table->count() * table->column_list->size());
    }
}

static PHP_METHOD(openswoole_table, getMemorySize) {
    Table *table = php_openswoole_table_get_ptr(ZEND_THIS);
    if (!table) {
        RETURN_LONG(0);
    } else {
        RETURN_LONG(table->get_memory_size());
    }
}

static PHP_METHOD(openswoole_table, getSize) {
    Table *table = php_openswoole_table_get_ptr(ZEND_THIS);
    if (!table) {
        RETURN_LONG(0);
    } else {
        RETURN_LONG(table->get_size());
    }
}

static PHP_METHOD(openswoole_table, rewind) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    table->rewind();
    table->forward();
}

static PHP_METHOD(openswoole_table, valid) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    auto key = table->current();
    RETURN_BOOL(key->key_len != 0);
}

static PHP_METHOD(openswoole_table, current) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    auto row = table->current();
    if (row->key_len == 0) {
        RETURN_NULL();
    }
    php_openswoole_table_row2array(table, row, return_value);
}

static PHP_METHOD(openswoole_table, key) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    auto row = table->current();
    if (row->key_len == 0) {
        RETURN_NULL();
    }
    RETVAL_STRINGL(row->key, row->key_len);
}

static PHP_METHOD(openswoole_table, next) {
    Table *table = php_openswoole_table_get_and_check_ptr2(ZEND_THIS);
    table->forward();
}
