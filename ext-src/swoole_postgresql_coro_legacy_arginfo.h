/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 206a8cca7282a53dc286a01e2d2ac5b2a0e5e5e3 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQL___destruct arginfo_class_Swoole_Coroutine_PostgreSQL___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_connect, 0, 0, 1)
	ZEND_ARG_INFO(0, conninfo)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_escape, 0, 0, 1)
	ZEND_ARG_INFO(0, string)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQL_escapeLiteral arginfo_class_Swoole_Coroutine_PostgreSQL_escape

#define arginfo_class_Swoole_Coroutine_PostgreSQL_escapeIdentifier arginfo_class_Swoole_Coroutine_PostgreSQL_escape

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_query, 0, 0, 1)
	ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQL_prepare arginfo_class_Swoole_Coroutine_PostgreSQL_query

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_metaData, 0, 0, 1)
	ZEND_ARG_INFO(0, table_name)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQL_createLOB arginfo_class_Swoole_Coroutine_PostgreSQL___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_openLOB, 0, 0, 1)
	ZEND_ARG_INFO(0, oid)
	ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_unlinkLOB, 0, 0, 1)
	ZEND_ARG_INFO(0, oid)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQL_status arginfo_class_Swoole_Coroutine_PostgreSQL___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQL_reset, 0, 0, 0)
	ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQLStatement_execute, 0, 0, 0)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchAll, 0, 0, 0)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQLStatement_affectedRows arginfo_class_Swoole_Coroutine_PostgreSQL___construct

#define arginfo_class_Swoole_Coroutine_PostgreSQLStatement_numRows arginfo_class_Swoole_Coroutine_PostgreSQL___construct

#define arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fieldCount arginfo_class_Swoole_Coroutine_PostgreSQL___construct

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchObject, 0, 0, 0)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, class_name)
	ZEND_ARG_INFO(0, ctor_params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchAssoc, 0, 0, 0)
	ZEND_ARG_INFO(0, row)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchArray, 0, 0, 0)
	ZEND_ARG_INFO(0, row)
	ZEND_ARG_INFO(0, result_type)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchRow arginfo_class_Swoole_Coroutine_PostgreSQLStatement_fetchArray
