--TEST--
openswoole_http_client_coro/websocket: ssl recv [2]
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort(), true);
        $ret = $cli->upgrade('/realtime');
        if (!$ret) {
            echo "ERROR\n";
            return;
        }
        echo "CONNECT SUCCESS, StatusCode={$cli->getStatusCode()}\n";
        $n = 16;
        $cli->push('{"op": "subscribe", "args": ["orderBookL2_25:XBTUSD"]}');
        while ($n--) {
            $frame = $cli->recv();
            if (!$frame or empty($frame->data)) {
                echo "ERROR $n [2]\n";
                var_dump($cli->errCode, $cli->errMsg);
                break;
            }
        }
        echo "FINISH\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $ws = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $ws->set([
        'log_file' => '/dev/null',
        'ssl_cert_file' => SSL_FILE_DIR . '/server.crt',
        'ssl_key_file' => SSL_FILE_DIR . '/server.key',
    ]);
    $ws->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $ws->on('message', function ($server, $frame) {
        for ($i = 0; $i < 16; $i++) {
            $server->push($frame->fd, json_encode(['table' => 'orderBookL2_25', 'data' => str_repeat('A', 32 * 1024 + $i)]));
        }
    });
    $ws->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
CONNECT SUCCESS, StatusCode=101
FINISH
