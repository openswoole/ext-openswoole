--TEST--
openswoole_websocket_server: frame length
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\WebSocket\Server;

$pm = new SwooleTest\ProcessManager;
$starts = new OpenSwoole\Atomic(0);
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $handshake = "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\n" .
            "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n";

        // a 64-bit payload length close to 2^64
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $cli->send($handshake);
        Assert::contains($cli->recv(), '101 Switching Protocols');
        $cli->send(hex2bin('81FFFFFFFFFFFFFFFFF8') . "\x01\x02\x03\x04" . str_repeat('A', 64));
        $data = $cli->recv(5);
        Assert::true($data === '' || ($data === false && $cli->errCode !== SOCKET_ETIMEDOUT));
        echo "closed\n";

        // the same worker still serves requests
        $ws = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($ws->upgrade('/'));
        $ws->push('hello');
        echo $ws->recv()->data, "\n";

        // a frame shorter than its declared length can not be unpacked
        var_dump(Server::unpack(hex2bin('817E0100') . 'short'));
        var_dump(Server::unpack(hex2bin('81FF7FFFFFFFFFFFFFFF') . "\x01\x02\x03\x04"));
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm, $starts) {
    $server = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set(['log_file' => '/dev/null', 'worker_num' => 1]);
    $server->on('workerStart', function () use ($pm, $starts) {
        if ($starts->add(1) == 1) {
            $pm->wakeup();
        }
    });
    $server->on('message', function ($server, $frame) use ($starts) {
        $server->push($frame->fd, "{$frame->data} worker_starts={$starts->get()}");
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
closed
hello worker_starts=1
bool(false)
bool(false)
