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
		public function end(string $data): bool {}
		public function sendfile(string $filename, int $offset = 0, int $length = 0): bool {}
		public function redirect(string $url, int $status_code = 302): void {}
		public function cookie(string $key, string $value = "", int $expire = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false, string $samesite = "", string $priority = ""): false|void {}
		public function rawcookie(string $key, string $value = "", int $expire = 0, string $path = "", string $domain = "", bool $secure = false, bool $httponly = false, string $samesite = "", string $priority = ""): false|void {}
		public function header(string $key, string $value, bool $format = true): false|void {}
		public function initHeader(): bool {}
		public function isWritable(): bool {}
		public function detach(): bool {}
		public function create(int $fd): \Swoole\Http\Response {}
		public function upgrade(): bool {}
		public function push(int $fd, string $data, int $opcode = SWOOLE_WEBSOCKET_OPCODE_TEXT, int $flags = SWOOLE_WEBSOCKET_FLAG_FIN): bool {}
		public function recv(float $timeout = 0): mixed {}
		public function close(): bool {}
		public function trailer(string $key, string $value): bool {}
		public function ping(): bool {}
		public function goaway(int $error_code = SWOOLE_HTTP2_ERROR_NO_ERROR, string $debug_data): bool {}
		public function status(int $status_code, string $reason = ""): bool {}
		public function __destruct() {}
	}
}