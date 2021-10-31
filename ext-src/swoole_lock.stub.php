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
	final class Lock {
		public function __construct(?long $type = 0, ?string $filename = null) {}
		public function lock(): bool {}
		public function lockwait(?float $timeout = 1.0): bool {}
		public function trylock(): bool {}
		public function lock_read(): bool {}
		public function trylock_read(): bool {}
		public function unlock(): bool {}
		public function destroy(): void {}
		public function __destruct() {}

	}
}