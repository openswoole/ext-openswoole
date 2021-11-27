<?php
/*
 +----------------------------------------------------------------------+
 | Open Swoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2021-now Open Swoole Group                             |
 +----------------------------------------------------------------------+
 | This source file is subject to version 2.0 of the Apache license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.apache.org/licenses/LICENSE-2.0.html                      |
 | If you did not receive a copy of the Apache2.0 license and are unable|
 | to obtain it through the world-wide-web, please send a note to       |
 | hello@swoole.co.uk so we can mail you a copy immediately.            |
 +----------------------------------------------------------------------+
*/

/** @not-serializable */
namespace Swoole\Coroutine {
    class Redis {
	    public function __construct(array $setting = []) {}
	    public function __destruct() {}
	    public function connect(string $host, int $port, bool $serialize = false): bool {}
	    public function getAuth(): mixed {}
	    public function getDBNum(): bool|int {}
	    public function getOptions(): array {}
	    public function setOptions(array $options): bool {}
	    public function getDefer(): bool {}
	    public function setDefer(bool $defer = true): bool {}
	    public function recv(): mixed {}
	    public function request(array $params): void {}
	    public function close(): bool {}
	    public function set(string $key, mixed $value, mixed $opt = null): bool|Redis {}
	    public function setBit(string $key, int $idx, bool $value): int|Redis {}
	    public function setEx(string $key, int $expire, mixed $value): bool|Redis {}
	    public function psetEx(string $key, int $expire, mixed $value): bool|Redis {}
	    public function lSet(string $key, int $index, string $value): bool {}
	    public function get(string $key): string|Redis {}
	    public function mGet(array $keys): array|Redis {}
	    public function del(array|string $key, string ...$other_keys): int|Redis {}
	    public function hDel(string $key, string $member, string ...$other_members): int {}
	    public function hSet(string $key, string $member, string $value): int {}
	    public function hMSet(string $key, array $keyvals): bool {}
	    public function hSetNx(string $key, string $member, string $value): int {}
	    public function delete(array|string $key, string ...$other_keys): int|Redis {}
	    public function mSet(array $key_values): bool {}
	    public function mSetNx(array $key_values): int {}
	    public function getKeys(string $pattern): array|Redis {}
	    public function keys(string $pattern): array|Redis {}
	    // openswoole fix
	    public function exists(string $key): bool|Redis {}
	    public function type(string $key): int|Redis {}
	    public function strLen(string $key): int|Redis {}
	    public function lPop(string $key): string|Redis {}
	    public function blPop(string|array $key, string|int $timeout_or_key, mixed ...$extra_args): array {}
	    public function rPop(string $key): string|Redis {}
	    public function brPop(string|array $key, string|int $timeout_or_key, mixed ...$extra_args): array {}
	    public function bRPopLPush(string $src, string $dst, int $timeout): string {}
	    // openswoole fix
	    public function lSize(string $key): int {}
	    public function lLen(string $key): int {}
	    // fix
	    public function sSize(string $key): int {}
	    public function scard(string $key): int {}
	    // fix
	    public function sPop(string $key, int $count = 0): string|array {}
	    public function sMembers(string $key): array {}
	    // fix
	    public function sGetMembers(string $key): array {}
	    public function sRandMember(string $key, int $count = 0): string|array {}
	    public function persist(string $key): bool {}
	    public function ttl(string $key): int {}
	    public function pttl(string $key): int {}
	    public function zCard(string $key): int {}
	    // fix
	    public function zSize(string $key): int {}
	    public function hLen(string $key): int {}
	    public function hKeys(string $key): array {}
	    public function hVals(string $key): array {}
	    public function hGetAll(string $key): array {}
	    public function debug(string $key): string {}
	    public function restore(string $key, int $timeout, string $value): bool {}
	    public function dump(string $key): string {}
	    // fix
	    public function renameKey(string $key_src, string $key_dst): bool|Redis {}
	    public function rename(string $key_src, string $key_dst): bool|Redis {}
	    public function renameNx(string $key_src, string $key_dst): bool|Redis {}
	    public function rpoplpush(string $src, string $dst): string {}
	    public function randomKey(): string|Redis {}
	    public function pfadd(string $key, array $elements): int {}
	    public function pfcount(string $key): int {}
	    public function pfmerge(string $dst, array $keys): bool {}
	    public function ping(?string $key = null): string|Redis {}
	    public function auth(mixed $credentials): bool {}
	    public function unwatch(): bool|Redis {}
	    public function watch(array|string $key, string ...$other_keys): bool|Redis {}
	    public function save(): bool {}
	    public function bgSave(): bool {}
	    public function lastSave(): int {}
	    // fix
	    public function flushDB(bool $async = false): bool {}
	    public function flushAll(bool $async = false): bool {}
	    public function dbSize(): int {}
	    public function bgrewriteaof(): bool {}
	    public function time(): array {}
	    public function role(): mixed {}
	    public function setRange(string $key, int $start, string $value): int|Redis {}
	    public function setNx(string $key, mixed $value): bool|array|Redis{}
	    public function getSet(string $key, mixed $value): string|Redis {}
	    public function append(string $key, mixed $value): int|Redis {}
	    public function lPushx(string $key, mixed $value): int|Redis {}
	    public function lPush(string $key, mixed $value): int|Redis {}
	    public function rPush(string $key, mixed $value): int|Redis {}
	    public function rPushx(string $key, mixed $value): int|Redis {}
	    // fix
	    public function sContains(string $key, mixed $value): bool {}
	    public function sismember(string $key, mixed $value): bool {}
	    public function zScore(string $key, mixed $member): float {}
	    public function zRank(string $key, string $member): int {}
	    public function zRevRank(string $key, string $member): int {}
	    public function hGet(string $key, string $member): string {}
	    public function hMGet(string $key, array $keys): array {}
	    public function hExists(string $key, string $member): bool {}
	    public function publish(string $channel, string $message): int {}
	    public function zIncrBy(string $key, float $value, mixed $member): float {}
	    public function zAdd(string $key, int $score, string $value): int {}
	    public function zPopMin(string $key, ?int $value = null): array {}
	    public function zPopMax(string $key, ?int $value = null): array {}
	    public function bzPopMin(string|array $key, string|int $timeout_or_key, mixed ...$extra_args): array {}
	    public function bzPopMax(string|array $key, string|int $timeout_or_key, mixed ...$extra_args): array {}
	    public function zDeleteRangeByScore(string $key, string $start, string $end): int {}
	    public function zRemRangeByScore(string $key, string $start, string $end): int {}
	    public function zCount(string $key, string $start , string $end): int {}
	    public function zRange(string $key, string $start, string $end, mixed $scores = null): array {}
	    public function zRevRange(string $key, string $start, string $end, mixed $scores = null): array {}
	    public function zRangeByScore(string $key, string $start, string $end, array $options = []): array {}
	    public function zRevRangeByScore(string $key, string $start, string $end, array $options = []): array {}
	    public function zRangeByLex(string $key, string $min, string $max, int $offset = -1, int $count = -1): array {}
	    public function zRevRangeByLex(string $key, string $min, string $max, int $offset = -1, int $count = -1): array {}
	    public function zInter(array $keys, array $weights = null, array $options = null): array {}
	    public function zinterstore(string $dst, array $keys, array $weights = null, string $aggregate = null): int {}
	    public function zUnion(array $keys, array $weights = null, array $options = null): array {}
	    public function zunionstore(string $dst, array $keys, array $weights = null, string $aggregate = null): int {}
	    public function incrBy(string $key, int $value): int|Redis {}
	    public function hIncrBy(string $key, string $member, int $value): int {}
	    public function incr(string $key): int|Redis {}
	    public function decrBy(string $key, int $value): int|Redis {}
	    public function decr(string $key): int|Redis {}
	    public function getBit(string $key, int $idx): int|Redis {}
	    public function lInsert(string $key, int $pos, mixed $pivot, mixed $value): int|Redis {}
	    // fix openswoole
	    public function lGet(string $key, int $index): string {}
	    public function lIndex(string $key, int $index): string {}
	    public function setTimeout(string $key, int $timeout): bool {}
	    public function expire(string $key, int $timeout): bool {}
	    public function pexpire(string $key, int $timeout): bool {}
	    public function expireAt(string $key, int $timeout): bool {}
	    public function pexpireAt(string $key, int $timeout): bool {}
	    public function move(string $key, int $index): bool {}
	    public function select(int $db): bool {}
	    public function getRange(string $key, int $start, int $end): string|Redis {}
	    public function listTrim(string $key, int $start , int $end): bool {}
	    public function ltrim(string $key, int $start , int $end): bool {}
	    public function lGetRange(string $key, int $start , int $end): array {}
	    public function lRange(string $key, int $start , int $end): array {}
	    public function lRem(string $key, string $value, int $count = 0): bool {}
	    public function lRemove(string $key, string $value, int $count = 0): bool {}
	    public function zDeleteRangeByRank(string $key, int $start, int $end): int {}
	    public function zRemRangeByRank(string $key, int $start, int $end): int {}
	    public function incrByFloat(string $key, float $value): int|Redis {}
	    public function hIncrByFloat(string $key, string $member, float $value): float {}
	    public function bitCount(string $key, int $start = 0, int $end = -1): int|Redis {}
	    public function bitOp(string $operation, string $deskey, string $srckey, string ...$other_keys): int {}
	    public function sAdd(string $key, mixed $value, mixed ...$other_values): int {}
	    public function sMove(string $src, string $dst, mixed $value): bool {}
	    public function sDiff(string $key, string ...$other_keys): array {}
	    public function sDiffStore(string $dst, string $key, string ...$other_keys): int {}
	    public function sUnion(string $key, string ...$other_keys): array {}
	    public function sUnionStore(string $dst, string $key, string ...$other_keys): int {}
	    public function sInter(string $key, string ...$other_keys): array {}
	    public function sInterStore(string $dst, string $key, string ...$other_keys): int {}
	    //fix
	    public function sRemove(string $key, string $value, string ...$other_values): int {}
	    public function srem(string $key, string $value, string ...$other_values): int {}
	    public function zDelete(string $key, string $member, string ...$other_members): int {}
	    public function zRemove(string $key, string $member, string ...$other_members): int {}
	    public function zRem(string $key, string $member, string ...$other_members): int {}
	    public function pSubscribe(array $patterns): void {}
	    public function subscribe(string $channel, string ...$other_channels): array {}
	    public function unsubscribe(string $channel, string ...$other_channels): array {}
	    public function pUnSubscribe(array $patterns): array {}
	    public function multi(int $value = \Redis::MULTI): bool|Redis {}
	    public function exec(): array {}
	    public function eval(string $script, array $keys = null, int $num_keys = 0): mixed {}
	    public function evalsha(string $sha1, array $keys = null, int $num_keys = 0): mixed {}
	    public function script(string $command, mixed ...$args): mixed {}
	}
}