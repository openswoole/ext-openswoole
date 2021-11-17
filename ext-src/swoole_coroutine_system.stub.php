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
		public function statvfs(string $path): bool|array {}
		public function readFile(string $filename, int $flags = 0): false|string {}
		public function writeFile(string $filename, string $data, int $flags = 0): bool {}
		public function wait(float $timeout = -1): bool {}
		public function waitPid(int $pid, float $timeout = -1): bool {}
		public function waitSignal(int $signalNum,  float $timeout = -1): bool {}
		public function waitEvent(mixed $fd, int $events, float $timeout = -1): bool|int {}
		public function fread(mixed $handle, int $length = 0): bool {}
		public function fgets(mixed $handle): bool|string {}
		public function fwrite(mixed $handle, string $data, int $length = 0): bool {}
		public function gethostbyname(string $domain, int $family = AF_INET, float $timeout = -1): string|false {}
		public function dnsLookup(string $domain, float $timeout = 5): string|false {}
		public function getaddrinfo(string $domain, int $family = AF_INET, int $sockType = SOCK_STREAM, int $protocol = STREAM_IPPROTO_TCP, string $service = null, float $timeout = -1): array|false{}
	}
}