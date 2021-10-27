<?php

namespace Swoole {

	/** @not-serializable */
	final class Timer
    {
    	public function set(array $settings): bool {}
		public function after(int $ms, callable $callback, mixed ...$params): bool|int {}
		public function tick(int $ms, callable $callback, mixed ...$params): bool|int {}
		public function exists(): bool {}
		public function info(): array|bool {}
		public function stats(): array {}
		public function list(): mixed {}
		public function clear(): bool {}
		public function clearAll(): bool {}
    }
}