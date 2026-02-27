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
namespace OpenSwoole {
    class Process {
        public function __construct(callable $callback, bool $redirectStdIO = false, int $pipeType = SOCK_DGRAM, bool $enableCoroutine = false) {}
        public function __destruct() {}
        public function useQueue(int $key = 0, int $mode = 2, int $capacity = -1): bool {}
        public function statQueue(): bool|array {}
        public function freeQueue(): bool {}
        public static function kill(int $pid, int $sigNo = SIGTERM): bool {}
        public static function signal(int $sigNo, ?callable $callback = null): bool {}
        public static function alarm(int $intervalUsec, int $type = ITIMER_REAL): bool {}
        public function start(): bool|int {}
        public function read(int $bufferSize = 8192): bool|string {}
        public function write(string $data): bool|int {}
        public function exportSocket(): \OpenSwoole\Coroutine\Socket|false {}
        public function push(string $data): bool {}
        public function pop(int $maxSize = 8192): bool|string {}
        public function exec(string $execFile , array $args): bool {}
        public static function daemon(bool $noChdir = true, bool $noClose = true, ?array $pipes = null): bool {}
        public function setAffinity(array $cpuSet): bool {}
        public function exit(int $status = 0): int {}
        public function close(int $reason = 0): bool {}
        public function set(array $settings): void {}
        public function setTimeout(float $timeout): void {}
        public function setBlocking(bool $blocking): void {}
        public static function setPriority(int $which, int $priority): bool {}
        public function getPriority(int $which): int {}
        public static function wait(bool $blocking = true): bool|array {}
        public function name(string $processName): bool {}
    }
}