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
		public function add(mixed $sock, ?callable $read_callback, ?callable $write_callback, ?int $flags): bool {}
		public function set(mixed $sock, ?callable $read_callback, ?callable $write_callback, ?int $flags): bool {}
		public function del(mixed $sock): bool {}
		public function isset(mixed $sock, ?int $flags): bool {}
		public function dispatch(): bool {}
		public function defer(callable $callback): bool {}
		public function cycle(callable $callback, ?bool $before = false): bool {}
		public function write(mixed $sock, string $data): bool {}
		public function wait(): void {}
		public function rshutdown(): void {}
		public function exit(): void {}
	}
}