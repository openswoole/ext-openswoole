<?php

/** @not-serializable */
namespace Swoole\Redis {
    class Server {
        public function setHandler(string $command, callable $callback): void {}
        public function getHandler(string $command): callable {}
        public function format(int $type, mixed $value = null): void {}
    }
}
