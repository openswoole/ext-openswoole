
ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis___construct, 0, 0, 0)
    ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_connect, 0, 0, 1)
    ZEND_ARG_INFO(0, host)
    ZEND_ARG_INFO(0, port)
    ZEND_ARG_INFO(0, serialize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setOptions, 0, 0, 1)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setDefer, 0, 0, 1)
    ZEND_ARG_INFO(0, defer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_key, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_key_value, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_key_long, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, integer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_request, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, params, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_append, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_auth, 0, 0, 1)
    ZEND_ARG_INFO(0, password)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bgSave, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bgrewriteaof, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bitCount, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bitOp, 0, 0, 3)
    ZEND_ARG_INFO(0, operation)
    ZEND_ARG_INFO(0, ret_key)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_blPop, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timeout_or_key)
    ZEND_ARG_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_brPop, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timeout_or_key)
    ZEND_ARG_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bRPopLPush, 0, 0, 3)
    ZEND_ARG_INFO(0, src)
    ZEND_ARG_INFO(0, dst)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_close, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_dbSize, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_debug, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_decr, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_decrBy, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_dump, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_eval, 0, 0, 1)
    ZEND_ARG_INFO(0, script)
    ZEND_ARG_INFO(0, args)
    ZEND_ARG_INFO(0, num_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_evalsha, 0, 0, 1)
    ZEND_ARG_INFO(0, script_sha)
    ZEND_ARG_INFO(0, args)
    ZEND_ARG_INFO(0, num_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_exec, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_exists, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_expireAt, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_flushAll, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_flushDB, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_get, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getBit, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getKeys, 0, 0, 1)
    ZEND_ARG_INFO(0, pattern)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getRange, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_getSet, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hDel, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, other_members)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hExists, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hSet, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hGetAll, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hIncrBy, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_incrByFloat, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hKeys, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hLen, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hMGet, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hMSet, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, pairs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hSetNx, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_hSetNx, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_hVals, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_incr, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_incrBy, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_incrByFloat, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lGet, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lGetRange, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lInsert, 0, 0, 4)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, position)
    ZEND_ARG_INFO(0, pivot)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lPop, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lPush, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lPushx, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_lRemove, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lSet, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, index)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lSize, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lastSave, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_listTrim, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, stop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_move, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, dbindex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_mSet, 0, 0, 1)
    ZEND_ARG_INFO(0, pairs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_mSetNx, 0, 0, 1)
    ZEND_ARG_INFO(0, pairs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_multi, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_persist, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_pexpire, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_pexpireAt, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timestamp)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pfadd, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, elements)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pfcount, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pfmerge, 0, 0, 2)
    ZEND_ARG_INFO(0, dstkey)
    ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_ping, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_psetEx, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, expire)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pSubscribe, 0, 0, 1)
    ZEND_ARG_INFO(0, patterns)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_subscribe, 0, 0, 1)
    ZEND_ARG_INFO(0, patterns)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_pttl, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_publish, 0, 0, 2)
    ZEND_ARG_INFO(0, channel)
    ZEND_ARG_INFO(0, message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rPop, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rPush, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rPushx, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_randomKey, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_renameKey, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, newkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_renameNx, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, newkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_restore, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, ttl)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_role, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rpoplpush, 0, 0, 2)
    ZEND_ARG_INFO(0, src)
    ZEND_ARG_INFO(0, dst)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sAdd, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sContains, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sDiff, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sDiffStore, 0, 0, 2)
    ZEND_ARG_INFO(0, dst)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sInter, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sInterStore, 0, 0, 2)
    ZEND_ARG_INFO(0, dst)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sMembers, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sMove, 0, 0, 3)
    ZEND_ARG_INFO(0, src)
    ZEND_ARG_INFO(0, dst)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sPop, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sRandMember, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sRemove, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sSize, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sUnion, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_sUnionStore, 0, 0, 2)
    ZEND_ARG_INFO(0, dst)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_save, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_script, 0, 0, 1)
    ZEND_ARG_INFO(0, cmd)
    ZEND_ARG_INFO(0, args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_select, 0, 0, 1)
    ZEND_ARG_INFO(0, dbindex)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_set, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, timeout)
    ZEND_ARG_INFO(0, opt)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setBit, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setRange, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setTimeout, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setEx, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, expire)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_setNx, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_strLen, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_unsubscribe, 0, 0, 1)
    ZEND_ARG_INFO(0, channels)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_time, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_ttl, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_type, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_unwatch, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_watch, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zAdd, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, score)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zPopMin, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zPopMax, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bzPopMin, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timeout_or_key)
    ZEND_ARG_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_bzPopMax, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, timeout_or_key)
    ZEND_ARG_INFO(0, extra_args)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zCard, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zCount, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zDelete, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, other_members)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByRank, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zDeleteRangeByScore, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zIncrBy, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zInter, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, keys)
    ZEND_ARG_INFO(0, weights)
    ZEND_ARG_INFO(0, aggregate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRange, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
    ZEND_ARG_INFO(0, scores)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRangeByLex, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRangeByScore, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRank, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRevRank, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
    ZEND_ARG_INFO(0, scores)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRevRangeByLex, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
    ZEND_ARG_INFO(0, offset)
    ZEND_ARG_INFO(0, limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_zRevRangeByScore, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_zRevRank, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zScore, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zUnion, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, keys)
    ZEND_ARG_INFO(0, weights)
    ZEND_ARG_INFO(0, aggregate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_del, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, other_keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lLen, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lRange, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_lRem, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_ltrim, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, stop)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_mGet, 0, 0, 1)
    ZEND_ARG_INFO(0, keys)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_rename, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, newkey)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_scard, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_zRem, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, other_members)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_zRemRangeByRank, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zRemRangeByScore, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_zRemove, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, member)
    ZEND_ARG_INFO(0, other_members)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zSize, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zinterstore, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, keys)
    ZEND_ARG_INFO(0, weights)
    ZEND_ARG_INFO(0, aggregate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Swoole_Coroutine_Redis_zunionstore, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, keys)
    ZEND_ARG_INFO(0, weights)
    ZEND_ARG_INFO(0, aggregate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_swoole_redis_coro_void, 0, 0, 0)
ZEND_END_ARG_INFO()


#define arginfo_class_Swoole_Coroutine_Redis___destruct arginfo_swoole_redis_coro_void
#define arginfo_class_Swoole_Coroutine_Redis_getAuth arginfo_swoole_redis_coro_void
#define arginfo_class_Swoole_Coroutine_Redis_getDBNum arginfo_swoole_redis_coro_void
#define arginfo_class_Swoole_Coroutine_Redis_getOptions arginfo_swoole_redis_coro_void
#define arginfo_class_Swoole_Coroutine_Redis_getDefer arginfo_swoole_redis_coro_void
#define arginfo_class_Swoole_Coroutine_Redis_recv arginfo_swoole_redis_coro_void

#define arginfo_class_Swoole_Coroutine_Redis_sGetMembers arginfo_class_Swoole_Coroutine_Redis_sMembers
#define arginfo_class_Swoole_Coroutine_Redis_hGet arginfo_class_Swoole_Coroutine_Redis_getSet
#define arginfo_class_Swoole_Coroutine_Redis_zRevRange arginfo_class_Swoole_Coroutine_Redis_zRange
#define arginfo_class_Swoole_Coroutine_Redis_zRevRangeByScore arginfo_class_Swoole_Coroutine_Redis_zRangeByScore
#define arginfo_class_Swoole_Coroutine_Redis_lIndex arginfo_class_Swoole_Coroutine_Redis_lGet
#define arginfo_class_Swoole_Coroutine_Redis_hIncrByFloat arginfo_class_Swoole_Coroutine_Redis_hIncrBy
#define arginfo_class_Swoole_Coroutine_Redis_pUnSubscribe arginfo_class_Swoole_Coroutine_Redis_unsubscribe
