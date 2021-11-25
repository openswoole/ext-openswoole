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
namespace Swoole {
	final class Coroutine {
		public static function create(callable $callback): mixed {}
		public static function set(array $options): mixed {}
		public static function getOptions(): array|null {}
		public static function exists(int $cid): bool {}
		public static function resume(int $cid): bool {}
		public static function defer(callable $callback): void {}
		public static function yield(): bool {}
		public static function suspend(): bool {}
		public static function cancel(int $cid): bool {}
		public static function isCanceled(): bool {}
		public static function stats(): array {}
		public static function getCid(): int {}
		public static function getPcid(int $cid = 0): int {}
		public static function getContext(int $cid = 0): mixed {}
		public static function getBackTrace(int $cid = 0, int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array {}
		public static function printBackTrace(int $cid = 0, int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, int $limit = 0): array {}
		public static function getElapsed(int $cid = 0): float {}
		public static function getStackUsage(int $cid = 0): int {}
		public static function list(): \Swoole\Coroutine\Iterator {}
		public static function enableScheduler(): bool {}
		public static function disableScheduler(): bool {}

		public static function exec(string $command, bool $get_error_stream): bool {}
		public static function sleep(float $seconds): bool {}
		public static function statvfs(string $path): bool|array {}
		public static function readFile(string $filename, int $flags = 0): false|string {}
		public static function writeFile(string $filename, string $data, int $flags = 0): bool {}
		public static function wait(float $timeout = -1): bool {}
		public static function waitPid(int $pid, float $timeout = -1): bool {}
		public static function waitSignal(int $signalNum,  float $timeout = -1): bool {}
		public static function waitEvent(mixed $fd, int $events, float $timeout = -1): bool|int {}
		public static function fread(mixed $handle, int $length = 0): string|false {}
		public static function fgets(mixed $handle): bool|string {}
		public static function fwrite(mixed $handle, string $data, int $length = 0): int|false {}
		public static function gethostbyname(string $domain, int $family = AF_INET, float $timeout = -1): string|false {}
		public static function dnsLookup(string $domain, float $timeout = 5): string|false {}
		public static function getaddrinfo(string $domain, int $family = AF_INET, int $sockType = SOCK_STREAM, int $protocol = STREAM_IPPROTO_TCP, string $service = null, float $timeout = -1): array|false{}
	}
}
