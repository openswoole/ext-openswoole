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
namespace Swoole\Coroutine {
	final class System {
		public function exec(string $command, bool $get_error_stream): bool {}
		public function sleep(float $seconds): bool {}
		public function getaddrinfo(string $host, ?int $family, ?int $socktype, ?int $protocol, ?string $service, ?float $timeout): bool|array {}
		public function statvfs(string $path): bool|array {}
		public function readFile(string $filename, ?int $flags): bool {}
		public function writeFile(string $filename, string $data, ?int $flags): bool {}
		public function wait(?float $timeout = -1): bool {}
		public function waitPid(int $pid, ?float $timeout = -1): bool {}
		public function waitSignal(int $signo,  ?float $timeout = -1): bool {}
		public function waitEvent(mixed $fd, ?int $events, ?float $timeout = -1): bool|int {}
		public function fread(mixed $handle, ?int $length = 0): bool {}
		public function fgets(mixed $handle): bool|string {}
		public function fwrite(mixed $handle, string $data, ?int $length = 0): bool {}
	}
}