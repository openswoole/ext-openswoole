<?php

/** @not-serializable */
namespace Swoole\Coroutine {
	final class Channel {
		public function __construct(?int $capacity = 1) {}
		public function push(mixed $data, ?float $timeout = -1): bool {}
		public function pop(?float $timeout = -1): mixed|bool {}
		public function close(): bool {}
		public function length(): int {}
		public function stats(): array {}
		public function isEmpty(): bool {}
		public function isFull(): bool {}
	}
}