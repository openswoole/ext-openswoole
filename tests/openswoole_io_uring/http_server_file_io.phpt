--TEST--
openswoole_io_uring: HTTP server with file I/O using io_uring engine
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
        $tmpfile = tempnam(sys_get_temp_dir(), 'openswoole_iouring_srv_');
        file_put_contents($tmpfile, 'io_uring_server_data');
        $content = file_get_contents($tmpfile);
        unlink($tmpfile);
        $resp->header('Content-Type', 'text/plain');
        $resp->end($content);
    });
    $serv->start();
};

$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        // Single request
        $body = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        Assert::same($body, 'io_uring_server_data');

        // Concurrent requests
        $n = 3;
        $chan = new OpenSwoole\Coroutine\Channel($n);
        for ($i = 0; $i < $n; $i++) {
            go(function () use ($pm, $chan) {
                $body = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
                $chan->push($body === 'io_uring_server_data' ? 'ok' : 'fail');
            });
        }
        for ($i = 0; $i < $n; $i++) {
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
