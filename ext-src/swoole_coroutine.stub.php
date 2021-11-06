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
	final class Coroutine {
		public function exists(int $cid): bool {}
		public function resume(int $cid): bool {}
		public function yield(): bool {}
		public function cancel(int $cid): bool {}
		public function isCanceled(): bool {}
		public function stats(): array {}
		public function getCid(): int {}
		public function getPcid(?int $cid = 0): int {}
		public function getContext(?int $cid = 0): mixed {}
		public function getBackTrace(?int $cid = 0, ?int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, ?int $limit = 0): array {}
		public function printBackTrace(?int $cid = 0, ?int $options = DEBUG_BACKTRACE_PROVIDE_OBJECT, ?int $limit = 0): array {}
		public function getElapsed(?int $cid = 0): double {}
		public function getStackUsage(?int $cid = 0): int {}
		public function list(): array {}
		public function enableScheduler(): bool {}
		public function disableScheduler(): bool {}
	}
}