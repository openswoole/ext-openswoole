--TEST--
openswoole_websocket_server: upgrade request followed by a partial request (SIMPLE_MODE)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 1));
        $upgrade = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\n" .
            "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n";
        // the unfinished second request leaves the HTTP parse offset in the receive buffer
        $cli->send($upgrade . "GET /next HTTP/1.1\r\nHost: localhost\r\n");
        Assert::contains($cli->recv(2), '101 Switching Protocols');
        // keep sending after the upgrade
        for ($i = 0; $i < 16; $i++) {
            if (!$cli->send(str_repeat('A', 8192))) {
                break;
            }
        }
        // the leftover bytes are not a valid frame, the server closes the connection
        $data = $cli->recv(2);
        Assert::true($data === '' || ($data === false && $cli->errCode != SOCKET_ETIMEDOUT));
        $cli->close();

        $ws = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($ws->upgrade('/'));
        $ws->push('hello');
        Assert::same($ws->recv(2)->data, 'hello');
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
        $serv->push($frame->fd, $frame->data);
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
