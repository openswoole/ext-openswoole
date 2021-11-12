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
	final class Atomic {
		public function __construct(int $value = 0) {}
		public function add(int $value = 1): int {}
		public function sub(int $value = 1): int {}
		public function get(): int {}
		public function set(int $value): bool|void {}
		public function wait(float $timeout = 1.0): bool {}
		public function wakeup(int $count = 1): int {}
		public function cmpset(int $cmp_val = 0, int $new_val = 0): int {}
	}
}

/** @not-serializable */
namespace Swoole\Atomic {
    final class Long {
        public function __construct(int $value = 0) {}
		public function add(int $value = 1): int {}
		public function sub(int $value = 1): int {}
		public function get(): int {}
		public function set(int $value): bool|void {}
		public function cmpset(int $cmp_val = 0, int $new_val = 0): int {}
    }
}