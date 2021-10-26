<?php

/** @not-serializable */
class swoole_table {
	public function __construct(int $table_size, ?float $conflict_proportion = 1.0) {}
	public function column(string $name, string $type, ?int $size): void {}
	public function create(): bool {}
	public function destroy(): bool {}
	public function set(mixed $key, array $value): bool {}
	public function get(mixed $key, string $field): array|bool {}
	public function count(): int {}
	public function del(mixed $key): bool {}
	public function exists(mixed $key): bool {}
	public function incr(mixed $key, string $column, int $incrby): int {}
	public function decr(mixed $key, string $column, int $decrby): int {}
	public function getSize(): int {}
	public function getMemorySize(): int {}
	public function rewind(): void {}
	public function valid(): bool {}
	public function next(): array|null {}
	public function current(): array|null {}
	public function key(): string|null {}

}