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
    class Process {
        public function __construct(callable $callback, ?boolean $redirect_stdin_and_stdout = false, ?int $pipe_type = 2, ?bool $enable_coroutine = false) {}
        public function __destruct() {}
        public function useQueue(?int $key = 0, ?int $mode = 2, ?int $capacity = -1): bool {}
        public function statQueue(): bool|array {}
        public function freeQueue(): bool {}
        public function kill(int $pid, ?int $signo = SIGTERM): bool {}
        public function signal(int $signo, ?callable $callback): bool {}
        public function alarm(int $interval_usec, int $type = ITIMER_REAL): bool {}
        public function start(): bool {}
        public function read(?int $buffer_size = 8192): bool|string {}
        public function write(string $data): bool|int {}
        public function exportSocket(): mixed {}
        public function push(string $data): bool {}
        public function pop(?int $maxsize = 8192): bool|string {}
        public function exec(string $exec_file , array $args): bool {}
        public function daemon(?bool $nochdir = true, ?bool $noclose = true, ?array $pipes = null ): bool {}
        public function setAffinity(array $cpu_set): bool {}
        public function exit(?int $status = 0): int {}
        public function close(?int $reason = 0): bool {}
        public function set(array $settings): void {}
        public function setTimeout(float $timeout): void {}
        public function setBlocking(bool $blocking): void {}
        public function setPriority(int $which, int $priority): bool {}
        public function getPriority(int $which): int {}
        public function wait(?bool $blocking = 1): bool|array {}
    }
}