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
	final class Event {
		public static function add(mixed $sock, ?callable $readCallback = null, ?callable $writeCallback = null, int $flags = SWOOLE_EVENT_READ): bool|int {}
		public static function set(mixed $sock, ?callable $readCallback = null, ?callable $writeCallback = null, int $flags = SWOOLE_EVENT_READ): bool {}
		public static function del(mixed $sock): bool {}
		public static function isset(mixed $sock, int $flags = SWOOLE_EVENT_READ|SWOOLE_EVENT_WRITE): bool {}
		public static function dispatch(): bool {}
		public static function defer(callable $callback): bool {}
		public static function cycle(callable $callback, bool $before = false): bool {}
		public static function write(mixed $sock, string $data): bool {}
		public static function wait(): void {}
		public static function rshutdown(): void {}
		public static function exit(): void {}
	}
}