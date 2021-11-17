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
 
namespace Swoole {
    final class Server {
        public function __construct(string $host, int $port = 0, int $mode = SWOOLE_PROCESS, int $sock_type = SWOOLE_SOCK_TCP) {}
        public function __destruct() {}
        public function listen(string $host, int $port, int $sock_type): mixed {}
        public function addlistener(string $host, int $port, int $sock_type): mixed {}

        public function on(string $event_name, callable $callback): bool {}
        public function getCallback(string $event_name): mixed {}
        public function set(array $settings): bool {}
        public function start(): bool {}

        // TODO: FIX OPENSWOOLE
        public function send(int $fd, mixed $data, int $server_socket = -1): bool {}
        public function sendto(string $ip, int $port, string $data, int $server_socket = -1): bool {}

        public function sendwait(int $fd, string $data): bool {}

        public function exists(int $fd): bool {}
        public function exist(int $fd): bool {}
        public function protect(int $fd, bool $is_protected = true): bool {}

        public function sendfile(int $fd, string $filename, int $offset = 0, int $length = 0): bool {}

        public function close(int $fd, bool $reset = false): bool {}

        public function confirm(int $fd): bool {}
        public function pause(int $fd): bool {}
        public function resume(int $fd): bool {}
        public function reload(): bool {}
        public function shutdown(): bool {}

        public function task(mixed $data, int $worker_id = -1, ?callable $finish_callback = null): bool {}
        public function taskwait(mixed $data, float $timeout = 0.5, int $worker_id = -1): bool {}
        public function taskWaitMulti(array $tasks, float $timeout = 0.5): bool {}
        public function taskCo(array $tasks, float $timeout = 0.5): bool {}

        public function finish(mixed $data): bool {}
        
        public function stop(int $worker_id, bool $wait_reactor = false): bool {}
        public function getLastError(): mixed {}
        public function heartbeat(bool $close_connection = false): bool {}

        public function getClientInfo(int $fd, int $reactor_id = -1, bool $dont_check_connection = false): bool|array {}

        public function getClientList(int $start_fd = 0, int $total = 10): bool|array {}

        public function getWorkerId(): int {}
        public function getWorkerPid(): int {}
        public function getWorkerStatus(int $worker_id = -1): bool|int {}
        public function getManagerPid(): int {}
        public function getMasterPid(): int {}

        public function connection_info(int $fd, int $reactor_id = -1, bool $dont_check_connection = false): bool|array {}
        public function connection_list(int $start_fd = 0, int $total = 10): bool|array {}
        public function sendMessage(mixed $message, int $worker_id = -1): bool {}

        public function addProcess(\Swoole\Process $process): bool|int {}
        public function stats(): array {}
        public function getSocket(int $port = -1): mixed {}
        public function bind(int $fd, int $uid): bool {}

        public function after(int $ms, callable $callback): void {}
        public function tick(int $ms, callable $callback) : void{}
        public function clearTimer(): void {}
        public function defer(callable $callback): void {}
    }
}

namespace Swoole\Server {
    final class Task {
        public function finish(mixed $data): bool {}
        public function pack(mixed $data): bool {}
    }
}

namespace Swoole\Connection {
    final class Iterator {
        public function __construct() {}
        public function __destruct() {}
        /** @tentative-return-type */
        public function rewind(): void {}
        /** @tentative-return-type */
        public function valid(): bool {}
        /** @tentative-return-type */
        public function key(): mixed {}
        /** @tentative-return-type */
        public function current(): mixed {}
        /** @tentative-return-type */
        public function next(): void {}
        /** @tentative-return-type */
        public function offsetGet(mixed $key): mixed {}
        /** @tentative-return-type */
        public function offsetSet(mixed $key, mixed $value): void {}
        /** @tentative-return-type */
        public function offsetUnset(mixed $key): void {}
        /** @tentative-return-type */
        public function offsetExists(mixed $key): bool {}
        /** @tentative-return-type */
        public function count(): int {}
    }
}