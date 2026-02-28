<?php
/*
 +----------------------------------------------------------------------+
 | OpenSwoole                                                          |
 +----------------------------------------------------------------------+
 | Copyright (c) 2021-now OpenSwoole Group                             |
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
namespace OpenSwoole\WebSocket {
    class Server {
        public function push(int $fd, \OpenSwoole\WebSocket\Frame|string $data, int $opcode = Server::WEBSOCKET_OPCODE_TEXT, int $flags = Server::WEBSOCKET_FLAG_FIN): bool {}
        public function disconnect(int $fd, int $code = Server::WEBSOCKET_CLOSE_NORMAL, string $reason = ""): bool {}
    	public function isEstablished(int $fd): bool {}
    	public static function pack(\OpenSwoole\WebSocket\Frame|string $data, int $opcode = Server::WEBSOCKET_OPCODE_TEXT, int $flags = Server::WEBSOCKET_FLAG_FIN): string {}
    	public static function unpack(string $data): \OpenSwoole\WebSocket\Frame|false {}
    }

    class Frame {
    	public function __toString(): string {}
    	public static function pack(\OpenSwoole\WebSocket\Frame|string $data, int $opcode = Server::WEBSOCKET_OPCODE_TEXT, int $flags = Server::WEBSOCKET_FLAG_FIN): string{}
    	public static function unpack(string $data): \OpenSwoole\WebSocket\Frame|false{}
    }
}