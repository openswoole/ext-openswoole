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
namespace Swoole\Http {
	final class Response {
		public function write(string $data): bool {}
		public function end(?string $data = null): bool {}
		public function sendfile(string $fileName, int $offset = 0, int $length = 0): bool {}
		public function redirect(string $url, int $status_code = 302): ?bool {}
		public function cookie(string $key, ?string $value = null, int $expire = 0, string $path = "", string $domain = "", bool $secure = false, bool $httpOnly = false, string $sameSite = "", string $priority = ""): bool {}
		public function rawcookie(string $key, ?string $value = null, int $expire = 0, string $path = "", string $domain = "", bool $secure = false, bool $httpOnly = false, string $sameSite = "", string $priority = ""): bool {}
		public function header(string $key, string $value, bool $format = true): bool {}
		public function initHeader(): bool {}
		public function isWritable(): bool {}
		public function detach(): bool {}
		public static function create(mixed $server = -1, int $fd = -1): Response|bool {}
		public function upgrade(): bool {}
		public function push(\OpenSwoole\WebSocket\Frame|string $data, int $opcode = \OpenSwoole\WebSocket\Server::WEBSOCKET_OPCODE_TEXT, int $flags = \OpenSwoole\WebSocket\Server::WEBSOCKET_FLAG_FIN): bool {}
		public function recv(float $timeout = 0): \OpenSwoole\WebSocket\Frame|bool|string {}
		public function close(): bool {}
		public function trailer(string $key, string $value): bool {}
		public function ping(): bool {}
		public function goaway(int $errorCode = \OpenSwoole\Coroutine\Http2\Client::HTTP2_ERROR_NO_ERROR, string $debugData = ""): bool {}
		public function status(int $statusCode, string $reason = ""): bool {}
		public function __destruct() {}
	}
}