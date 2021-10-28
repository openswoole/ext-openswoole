<?php

/** @not-serializable */
namespace Swoole {
	final class Table {
		public function __construct(int $table_size, ?float $conflict_proportion = 1.0) {}
		public function column(string $name, int $type, ?int $size): void {}
		public function create(): bool {}
		public function destroy(): bool {}
		public function set(string $key, array $value): bool {}
		public function get(string $key, string $column): array|bool|string {}
		public function count(): int {}
		public function del(string $key): bool {}
		public function exists(string $key): bool {}
		public function incr(string $key, string $column, ?int $incrby = 1): int {}
		public function decr(string $key, string $column, ?int $decrby = 1): int {}
		public function getSize(): int {}
		public function getMemorySize(): int {}
		public function rewind(): void {}
		public function valid(): bool {}
		public function next(): void {}
		public function current(): array|null {}
		public function key(): string|null {}

	}
}