/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: cfcc0a3fedf523eac28612c33bcd9aaf9efa69c2 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, setting, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis___destruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_connect, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, serialize, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getAuth, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_getDBNum, 0, 0, MAY_BE_BOOL|MAY_BE_LONG)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getOptions, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setOptions, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getDefer, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setDefer, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, defer, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_recv arginfo_class_Swoole_Coroutine_Redis_getAuth

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_request, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_close arginfo_class_Swoole_Coroutine_Redis_getDefer

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_set, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opt, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_setBit, 0, 3, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, idx, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_setEx, 0, 3, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, expire, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_psetEx arginfo_class_Swoole_Coroutine_Redis_setEx

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lSet, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_get, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_mGet, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_del, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hDel, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_members, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hSet, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hMSet, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keyvals, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_hSetNx arginfo_class_Swoole_Coroutine_Redis_hSet

#define arginfo_class_Swoole_Coroutine_Redis_delete arginfo_class_Swoole_Coroutine_Redis_del

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_mSet, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_mSetNx, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key_values, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_getKeys, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, pattern, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_keys arginfo_class_Swoole_Coroutine_Redis_getKeys

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_exists, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_type, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_strLen arginfo_class_Swoole_Coroutine_Redis_type

#define arginfo_class_Swoole_Coroutine_Redis_lPop arginfo_class_Swoole_Coroutine_Redis_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_blPop, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_STRING|MAY_BE_ARRAY, NULL)
	ZEND_ARG_TYPE_MASK(0, timeout_or_key, MAY_BE_STRING|MAY_BE_LONG, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, extra_args, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_rPop arginfo_class_Swoole_Coroutine_Redis_get

#define arginfo_class_Swoole_Coroutine_Redis_brPop arginfo_class_Swoole_Coroutine_Redis_blPop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bRPopLPush, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lSize, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_lLen arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_sSize arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_scard arginfo_class_Swoole_Coroutine_Redis_lSize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_sPop, 0, 1, MAY_BE_STRING|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sMembers, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_sGetMembers arginfo_class_Swoole_Coroutine_Redis_sMembers

#define arginfo_class_Swoole_Coroutine_Redis_sRandMember arginfo_class_Swoole_Coroutine_Redis_sPop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_persist, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_ttl arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_pttl arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_zCard arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_zSize arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_hLen arginfo_class_Swoole_Coroutine_Redis_lSize

#define arginfo_class_Swoole_Coroutine_Redis_hKeys arginfo_class_Swoole_Coroutine_Redis_sMembers

#define arginfo_class_Swoole_Coroutine_Redis_hVals arginfo_class_Swoole_Coroutine_Redis_sMembers

#define arginfo_class_Swoole_Coroutine_Redis_hGetAll arginfo_class_Swoole_Coroutine_Redis_sMembers

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_debug, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_restore, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_dump arginfo_class_Swoole_Coroutine_Redis_debug

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_renameKey, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO(0, key_src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key_dst, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_rename arginfo_class_Swoole_Coroutine_Redis_renameKey

#define arginfo_class_Swoole_Coroutine_Redis_renameNx arginfo_class_Swoole_Coroutine_Redis_renameKey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rpoplpush, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_randomKey, 0, 0, Swoole\\Coroutine\\Redis, MAY_BE_STRING)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pfadd, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, elements, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_pfcount arginfo_class_Swoole_Coroutine_Redis_lSize

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pfmerge, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_ping, 0, 0, Swoole\\Coroutine\\Redis, MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, key, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_auth, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, credentials, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_unwatch, 0, 0, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_watch, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_MASK(0, key, MAY_BE_ARRAY|MAY_BE_STRING, NULL)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_save arginfo_class_Swoole_Coroutine_Redis_getDefer

#define arginfo_class_Swoole_Coroutine_Redis_bgSave arginfo_class_Swoole_Coroutine_Redis_getDefer

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lastSave, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_flushDB, 0, 0, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, async, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_flushAll arginfo_class_Swoole_Coroutine_Redis_flushDB

#define arginfo_class_Swoole_Coroutine_Redis_dbSize arginfo_class_Swoole_Coroutine_Redis_lastSave

#define arginfo_class_Swoole_Coroutine_Redis_bgrewriteaof arginfo_class_Swoole_Coroutine_Redis_getDefer

#define arginfo_class_Swoole_Coroutine_Redis_time arginfo_class_Swoole_Coroutine_Redis_getOptions

#define arginfo_class_Swoole_Coroutine_Redis_role arginfo_class_Swoole_Coroutine_Redis_getAuth

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_setRange, 0, 3, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_setNx, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_BOOL|MAY_BE_ARRAY)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_getSet, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_append, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_lPushx arginfo_class_Swoole_Coroutine_Redis_append

#define arginfo_class_Swoole_Coroutine_Redis_lPush arginfo_class_Swoole_Coroutine_Redis_append

#define arginfo_class_Swoole_Coroutine_Redis_rPush arginfo_class_Swoole_Coroutine_Redis_append

#define arginfo_class_Swoole_Coroutine_Redis_rPushx arginfo_class_Swoole_Coroutine_Redis_append

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sContains, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_sismember arginfo_class_Swoole_Coroutine_Redis_sContains

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zScore, 0, 2, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRank, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRevRank arginfo_class_Swoole_Coroutine_Redis_zRank

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hGet, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hMGet, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hExists, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_publish, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, channel, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zIncrBy, 0, 3, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zAdd, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, score, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zPopMin, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zPopMax arginfo_class_Swoole_Coroutine_Redis_zPopMin

#define arginfo_class_Swoole_Coroutine_Redis_bzPopMin arginfo_class_Swoole_Coroutine_Redis_blPop

#define arginfo_class_Swoole_Coroutine_Redis_bzPopMax arginfo_class_Swoole_Coroutine_Redis_blPop

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByScore, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRemRangeByScore arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByScore

#define arginfo_class_Swoole_Coroutine_Redis_zCount arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByScore

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRange, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, scores, IS_MIXED, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRevRange arginfo_class_Swoole_Coroutine_Redis_zRange

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRangeByScore, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRevRangeByScore arginfo_class_Swoole_Coroutine_Redis_zRangeByScore

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRangeByLex, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, min, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, max, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, offset, IS_LONG, 0, "-1")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRevRangeByLex arginfo_class_Swoole_Coroutine_Redis_zRangeByLex

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zInter, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, options, IS_ARRAY, 0, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zinterstore, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, keys, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, weights, IS_ARRAY, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, aggregate, IS_STRING, 0, "null")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zUnion arginfo_class_Swoole_Coroutine_Redis_zInter

#define arginfo_class_Swoole_Coroutine_Redis_zunionstore arginfo_class_Swoole_Coroutine_Redis_zinterstore

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_incrBy, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hIncrBy, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_incr arginfo_class_Swoole_Coroutine_Redis_type

#define arginfo_class_Swoole_Coroutine_Redis_decrBy arginfo_class_Swoole_Coroutine_Redis_incrBy

#define arginfo_class_Swoole_Coroutine_Redis_decr arginfo_class_Swoole_Coroutine_Redis_type

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_getBit, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, idx, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_lInsert, 0, 4, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pos, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, pivot, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lGet, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_lIndex arginfo_class_Swoole_Coroutine_Redis_lGet

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setTimeout, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_expire arginfo_class_Swoole_Coroutine_Redis_setTimeout

#define arginfo_class_Swoole_Coroutine_Redis_pexpire arginfo_class_Swoole_Coroutine_Redis_setTimeout

#define arginfo_class_Swoole_Coroutine_Redis_expireAt arginfo_class_Swoole_Coroutine_Redis_setTimeout

#define arginfo_class_Swoole_Coroutine_Redis_pexpireAt arginfo_class_Swoole_Coroutine_Redis_setTimeout

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_move, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_select, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, db, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_getRange, 0, 3, Swoole\\Coroutine\\Redis, MAY_BE_STRING)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_listTrim, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_ltrim arginfo_class_Swoole_Coroutine_Redis_listTrim

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lGetRange, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_lRange arginfo_class_Swoole_Coroutine_Redis_lGetRange

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lRem, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, count, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_lRemove arginfo_class_Swoole_Coroutine_Redis_lRem

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByRank, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, end, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_zRemRangeByRank arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByRank

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_incrByFloat, 0, 2, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hIncrByFloat, 0, 3, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, member, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_bitCount, 0, 1, Swoole\\Coroutine\\Redis, MAY_BE_LONG)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, start, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, end, IS_LONG, 0, "-1")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bitOp, 0, 3, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, operation, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, deskey, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, srckey, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sAdd, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sMove, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, src, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sDiff, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sDiffStore, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, dst, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_keys, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_sUnion arginfo_class_Swoole_Coroutine_Redis_sDiff

#define arginfo_class_Swoole_Coroutine_Redis_sUnionStore arginfo_class_Swoole_Coroutine_Redis_sDiffStore

#define arginfo_class_Swoole_Coroutine_Redis_sInter arginfo_class_Swoole_Coroutine_Redis_sDiff

#define arginfo_class_Swoole_Coroutine_Redis_sInterStore arginfo_class_Swoole_Coroutine_Redis_sDiffStore

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sRemove, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_values, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_srem arginfo_class_Swoole_Coroutine_Redis_sRemove

#define arginfo_class_Swoole_Coroutine_Redis_zDelete arginfo_class_Swoole_Coroutine_Redis_hDel

#define arginfo_class_Swoole_Coroutine_Redis_zRemove arginfo_class_Swoole_Coroutine_Redis_hDel

#define arginfo_class_Swoole_Coroutine_Redis_zRem arginfo_class_Swoole_Coroutine_Redis_hDel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pSubscribe, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_subscribe, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, channel, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, other_channels, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_unsubscribe arginfo_class_Swoole_Coroutine_Redis_subscribe

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pUnSubscribe, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, patterns, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_class_Swoole_Coroutine_Redis_multi, 0, 0, Swoole\\Coroutine\\Redis, MAY_BE_BOOL)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, value, IS_LONG, 0, "Redis::MULTI")
ZEND_END_ARG_INFO()

#define arginfo_class_Swoole_Coroutine_Redis_exec arginfo_class_Swoole_Coroutine_Redis_getOptions

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_eval, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, script, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keys, IS_ARRAY, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_evalsha, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, sha1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, keys, IS_ARRAY, 0, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_keys, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_script, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, command, IS_STRING, 0)
	ZEND_ARG_VARIADIC_TYPE_INFO(0, args, IS_MIXED, 0)
ZEND_END_ARG_INFO()
