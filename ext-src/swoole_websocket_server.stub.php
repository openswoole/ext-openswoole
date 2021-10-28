<?php

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