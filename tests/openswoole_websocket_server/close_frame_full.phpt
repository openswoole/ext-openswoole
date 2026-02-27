--TEST--
openswoole_websocket_server: websocket server send and recv close frame full test
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function (int $pid) use ($pm) {
    for ($c = 32; $c--;) {
        go(function () use ($pm) {
            $cli = new \OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
            $cli->set(['timeout' => 5]);
            for ($n = MAX_REQUESTS; $n--;) {
                $ret = $cli->upgrade('/');
                Assert::assert($ret);
                $code = mt_rand(0, 5000);
                $reason = md5((string)$code);
                $close_frame = new OpenSwoole\WebSocket\Frame;
                $close_frame->opcode = WEBSOCKET_OPCODE_CLOSE;
                $close_frame->code = $code;
                $close_frame->reason = $reason;
                $cli->push($close_frame);
                // recv the last close frame
                $frame = $cli->recv();
                Assert::isInstanceOf($frame, OpenSwoole\WebSocket\Frame::class);
                Assert::same($frame->opcode, WEBSOCKET_OPCODE_CLOSE);
                Assert::same(md5((string)$frame->code), $frame->reason);
                // connection closed
                Assert::false($cli->recv());
                Assert::false($cli->connected);
                Assert::same($cli->errCode, 0); // connection close normally
            }
        });
    }
    openswoole_event_wait();
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), SERVER_MODE_RANDOM);
    $serv->set([
        'log_file' => '/dev/null'
    ]);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
        Assert::isInstanceOf($frame, OpenSwoole\WebSocket\Frame::class);
        if ($frame->opcode !== WEBSOCKET_OPCODE_CLOSE) {
            return;
        }
        if (mt_rand(0, 1)) {
            $serv->push($frame->fd, $frame);
        } else {
            $serv->disconnect($frame->fd, $frame->code, $frame->reason);
        }
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
