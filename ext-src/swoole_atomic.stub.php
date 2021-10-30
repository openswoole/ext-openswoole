<?php

/** @not-serializable */
namespace Swoole {
	final class Atomic {
		public function __construct(?int $value = 0) {}
		public function add(?int $value = 1): int {}
		public function sub(?int $value = 1): int {}
		public function get(): int {}
		public function set(int $value): bool|void {}
		public function wait(?float $timeout = 1.0): bool {}
		public function wakeup(?int $count = 1): int {}
		public function cmpset(int $cmp_val = 0, int $new_val = 0): int {}
	}
}

/** @not-serializable */
namespace Swoole\Atomic {
    final class Long {
        public function __construct(?int $value = 0) {}
		public function add(?int $value = 1): int {}
		public function sub(?int $value = 1): int {}
		public function get(): int {}
		public function set(int $value): bool|void {}
		public function cmpset(int $cmp_val = 0, int $new_val = 0): int {}
    }
}