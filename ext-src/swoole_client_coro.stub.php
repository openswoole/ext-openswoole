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
	final class Client {
		public function __construct(int $type, bool $async = false, string $id = "") {}
		public function set(array $settings): bool {}
		public function connect(string $host, int $port = 0, float $timeout = 0.5, int $sock_flag = 0): bool {}		
		public function send(string $data, float $timeout = 1.0): bool|int {}
		public function sendto(string $host, int $port, string $data): bool {}
		public function sendfile(string $filename, int $offset = 0, int $length = 0): bool {}
		public function recv(float $timeout = 1.0): bool|string {}
		public function recvfrom(int $length, string &$host, int &$port = 0): bool|string {}
		public function peek(int $length = 65535): bool|string {}
		public function close(): bool {}
		public function isConnected(): bool {}
		public function getsockname(): bool|array {}
		public function exportSocket(): mixed {}
		public function getpeername(): bool|array {}
		public function enableSSL(): bool {}
		public function getPeerCert(): bool|string {}
		public function verifyPeerCert(): bool {}
		public function __destruct() {}
	}
}