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
	final class Client {
		public function __construct(int $sockType, bool $async = false, string $id = "") {}
		public function set(array $settings): bool {}
		public function connect(string $host, int $port, float $timeout = 0.5, int $sockFlag = 0): bool {}
		public function send(string $data, int $flags = 0): bool|int {}
		public function sendto(string $ip, int $port, string $data): bool {}
		public function sendfile(string $fileName, int $offset = 0, int $length = 0): bool {}
		public function recv(int $length = 65535, int $flags = 0): bool|string {}
		public function close(bool $force = false): bool {}
		public static function select(array &$readReady, array &$writeReady, array &$errors, float $timeout = 0.5 ): bool {}
		public function shutdown(int $how): bool {}
		public function isConnected(): bool {}
		public function getsockname(): bool|array {}
		public function getSocket(): \Socket|false {}
		public function getpeername(): bool|array {}
		public function enableSSL(): bool {}
		public function getPeerCert(): bool|string {}
		public function verifyPeerCert(): bool {}
		public function __destruct() {}
	}
}
