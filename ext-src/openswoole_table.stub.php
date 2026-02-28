<?php
/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2021-now OpenSwoole Group                             |
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
namespace OpenSwoole {
	final class Table {
		public function __construct(int $size, float $conflictProportion = 1.0) {}
		public function column(string $name, int $type, int $size = 0): bool {}
		public function create(): bool {}
		public function destroy(): bool {}
		public function set(string $key, array $value): bool {}
		public function get(string $key, string $column = ""): array|bool|string|int|float {}
		public function count(): int {}
		public function del(string $key): bool {}
		public function exists(string $key): bool {}
		public function incr(string $key, string $column, int $incrBy = 1): int {}
		public function decr(string $key, string $column, int $decrBy = 1): int {}
		public function getSize(): int {}
		public function getMemorySize(): int {}
		public function rewind(): void {}
		public function valid(): bool {}
		public function next(): void {}
		public function current(): ?array {}
		public function key(): ?string {}

	}
}