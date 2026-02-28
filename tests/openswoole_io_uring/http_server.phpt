--TEST--
openswoole_io_uring: HTTP server with io_uring reactor backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->childFunc = function () use ($pm) {
    Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
    $serv = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $serv->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
    ]);
    $serv->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('request', function ($req, $resp) {
        $resp->header('Content-Type', 'text/plain');
        $resp->end('hello from io_uring server');
    });
    $serv->start();
};

$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        // Basic GET request
        $body = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        Assert::same($body, 'hello from io_uring server');

        // Multiple concurrent requests
        $chan = new OpenSwoole\Coroutine\Channel(3);
        for ($i = 0; $i < 3; $i++) {
            go(function () use ($pm, $chan) {
                $body = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
                $chan->push($body === 'hello from io_uring server' ? 'ok' : 'fail');
            });
        }
        for ($i = 0; $i < 3; $i++) {
            Assert::same($chan->pop(), 'ok');
        }

        $pm->kill();
    });
    OpenSwoole\Event::wait();
    echo "DONE\n";
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
