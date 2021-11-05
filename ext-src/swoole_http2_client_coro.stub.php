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
namespace Swoole\Coroutine\Http2 {
	final class Client {
		public function __construct(string $host, int $port, bool $openSSL = false): mixed {}
		public function set(array $options): void {}
		public function connect(): bool {}
		public function stats(string $key = null): bool|array {}
		public function isStreamExist(int $streamId): bool {}
		public function send(Swoole\Http2\Request $request): bool|int {}
		public function write(int $streamId, mixed $data, bool $end = false): bool {}
		public function recv(float $timeout): Swoole\Http2\Response|bool {}
		public function read(float $timeout): Swoole\Http2\Response {}
		public function goaway(int $errorCode = SWOOLE_HTTP2_ERROR_NO_ERROR, string $debugData): bool {}
		public function ping(): bool {}
		public function close(): bool {}
		public function __destruct() {}
	}
}