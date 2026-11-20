--TEST--
openswoole_websocket_server: auto pong echoes a ping sent with the 16-bit length form
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

function masked_ping_frame16(string $payload): string
{
    $mask = random_bytes(4);
    $masked = '';
    for ($i = 0; $i < strlen($payload); $i++) {
        $masked .= $payload[$i] ^ $mask[$i % 4];
    }
    return "\x89" . chr(0x80 | 126) . pack('n', strlen($payload)) . $mask . $masked;
}

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 1));
        $cli->send("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\n" .
            "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n");
        $data = '';
        while (!str_contains($data, "\r\n\r\n")) {
            $chunk = $cli->recv(2);
            Assert::notEmpty($chunk);
            $data .= $chunk;
        }
        Assert::contains($data, '101 Switching Protocols');
        $cli->send(masked_ping_frame16('hello'));
        $frames = substr($data, strpos($data, "\r\n\r\n") + 4);
        while (strlen($frames) < 7) {
            $chunk = $cli->recv(2);
            Assert::notEmpty($chunk);
            $frames .= $chunk;
        }
        Assert::same(bin2hex($frames), bin2hex("\x8a\x05hello"));
        $cli->close();
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::SIMPLE_MODE);
    $serv->set(['worker_num' => 1, 'log_file' => '/dev/null']);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
