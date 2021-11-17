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

namespace Swoole\Coroutine\Http {
	final class Client {
	    public function __construct(string $host, int $port, bool $ssl = false) {}
	    public function __destruct() {}
	    public function set(array $settings): bool {}
	    public function getDefer(): bool {}
	    public function setDefer(bool $defer = false): void {}
	    public function setMethod(string $method): void {}
	    public function setHeaders(array $headers): void {}
	    public function setBasicAuth(string $username, string $password): void {}
	    public function setCookies(array $cookies): void {}
	    public function setData(string|array $data): void {}
	    public function addFile(string $path, string $name, string $mimeType = "", string $fileName = "", int $offset = 0, int $length = 0): void {}
	    public function addData(string $data, string $name, string $mimeType = "", string $filename = ""): void {}
	    public function execute(string $path): int|bool {}
	    public function getpeername(): array|false {}
	    public function getsockname(): array|false {}
	    public function get(string $path): void {}
	    public function post(string $path, mixed $data): void {}
	    public function download(string $path, string $filename,  int $offset = 0): bool {}
	    public function getBody(): string|bool {}
	    public function getHeaders(): array|bool {}
	    public function getCookies(): array|bool {}
	    public function getStatusCode(): int|bool {}
	    public function getHeaderOut(): string|bool {}
	    public function getPeerCert(): string|false {}
	    public function upgrade(string $path): bool {}
	    public function push(mixed $data, int $opcode = WEBSOCKET_OPCODE_TEXT, int $flags = SWOOLE_WEBSOCKET_FLAG_FIN): bool {}
	    public function recv(float $timeout = 0): bool|\Swoole\WebSocket\Frame {}
	    public function close(): bool {}
	}
}