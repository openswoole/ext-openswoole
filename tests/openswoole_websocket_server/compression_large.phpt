--TEST--
openswoole_websocket_server: compressed message size
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_function_not_exist('gzdeflate');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\WebSocket\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        // masked frame with an all-zero mask key, RSV1 set: a compressed message
        $frame = function (string $raw) {
            $payload = substr(gzdeflate($raw, 9, ZLIB_ENCODING_RAW), 0);
            return "\xC1\xFF" . pack('J', strlen($payload)) . "\0\0\0\0" . $payload;
        };
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $cli->send("GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n" .
            "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n");
        Assert::contains($cli->recv(), '101 Switching Protocols');
        // 8MB of zeros deflates to about 8KB
        $cli->send($frame(str_repeat("\0", 8 * 1024 * 1024)));
        $cli->send($frame(str_repeat('a', 100 * 1024)));
        $reply = $cli->recv(5);
        echo substr($reply, 2), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'websocket_compression' => true,
        'package_max_length' => 1024 * 1024,
    ]);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('message', function ($server, $frame) {
        $server->push($frame->fd, 'got ' . strlen($frame->data));
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
got 102400
