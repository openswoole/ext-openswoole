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
namespace Swoole\WebSocket {
    class Server {
        public function push(int $fd, string $data, int $opcode, int $flags = null): bool {}
        public function disconnect(int $fd, int $code, string $reason = null): bool {}
    	public function isEstablished(int $fd): bool {}
    	public static function pack(string $data, int $opcode = 1, int $flags = null): string {}
    	public static function unpack(string $data): bool {}
    }

    class Frame {
    	public function __toString(): string {}
    	public static function pack(string $data, int $opcode = 1, int $flags = 1): void{}
    	public static function unpack(string $data): void{}
    }
}