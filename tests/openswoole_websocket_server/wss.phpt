--TEST--
openswoole_websocket_server: websocket over tls (SIMPLE_MODE)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort(), true);
        $cli->set(['timeout' => 10]);
        Assert::true($cli->upgrade('/'));
        Assert::same($cli->statusCode, 101);

        $messages = [
            [WEBSOCKET_OPCODE_TEXT, 'hello'],
            [WEBSOCKET_OPCODE_BINARY, random_bytes(200 * 1024)],
            [WEBSOCKET_OPCODE_TEXT, str_repeat('t', 70000)],
            [WEBSOCKET_OPCODE_BINARY, random_bytes(126)],
        ];
        // one at a time
        foreach ($messages as [$opcode, $data]) {
            Assert::true($cli->push($data, $opcode));
            $frame = $cli->recv();
            Assert::isInstanceOf($frame, OpenSwoole\WebSocket\Frame::class);
            Assert::same($frame->opcode, $opcode);
            Assert::true($frame->finish);
            Assert::same(md5($frame->data), md5($data));
        }
        // all at once, the echoes come back in order
        foreach ($messages as [$opcode, $data]) {
            Assert::true($cli->push($data, $opcode));
        }
        foreach ($messages as [$opcode, $data]) {
            $frame = $cli->recv();
            Assert::isInstanceOf($frame, OpenSwoole\WebSocket\Frame::class);
            Assert::same($frame->opcode, $opcode);
            Assert::same(md5($frame->data), md5($data));
        }
        $cli->close();
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::SIMPLE_MODE,
        OpenSwoole\Constant::SOCK_TCP | OpenSwoole\Constant::SSL);
    $serv->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'open_websocket_protocol' => true,
        'ssl_cert_file' => SSL_FILE_DIR . '/server.crt',
        'ssl_key_file' => SSL_FILE_DIR . '/server.key',
    ]);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
        Assert::true($serv->push($frame->fd, $frame->data, $frame->opcode));
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
