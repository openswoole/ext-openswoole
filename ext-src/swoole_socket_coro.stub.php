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
    final class Socket {
        public function __construct(int $domain, int $type, int $protocol = IPPROTO_IP) {}
        public function bind(string $address, int $port = 0): bool {}
        public function listen(int $backlog = 512): bool {}
        public function accept(float $timeout = 0): Socket|false {}
        public function connect(string $host, int $port = 0, float $timeout = 0): mixed {}
        public function checkLiveness(): bool {}
        public function peek(int $length = 65536): bool|string {}
        public function recv(int $length = 65536, float $timeout = 0): bool|string {}
        public function recvAll(int $length = 65536, float $timeout = 0): bool|string {}
        public function recvLine(int $length = 65536, float $timeout = 0): bool|string {}
        public function recvWithBuffer(int $length = 65536, float $timeout = 0): bool|string {}
        public function recvPacket(float $timeout = 0): bool|string {}
        public function send(string $data, float $timeout = 0): bool|int {}
        public function readVector(array $ioVector, float $timeout = 0): bool|array {}
        public function readVectorAll(array $ioVector, float $timeout = 0): bool|array {}
        public function writeVector(array $ioVector, float $timeout = 0): bool|int {}
        public function writeVectorAll(array $ioVector, float $timeout = 0): bool|int {}
        public function sendFile(string $fileName, int $offset = 0, int $length = 0): bool {}
        public function sendAll(string $data, float $timeout = 0): bool|int {}
        public function recvfrom(mixed &$peerName, float $timeout = 0): bool|string {}
        public function sendto(string $addr, int $port, string $data): int|bool {}
        public function getOption(int $level, int $name): mixed {}
        public function setProtocol(array $settings): bool {}
        public function setOption(int $level, int $name, mixed $value): bool {}
        public function sslHandshake(): bool {}
        public function shutdown(int $how = 0): bool {}
        public function cancel(int $event = Socket::EVENT_READ): bool {}
        public function close(): bool {}
        public function getpeername(): bool|array {}
        public function getsockname(): bool|array {}
    }
}
