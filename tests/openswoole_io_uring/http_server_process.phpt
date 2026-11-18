--TEST--
openswoole_io_uring: HTTP server in process mode with the io_uring reactor
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// the worker pipes get consecutive fds: run twice, the second time with one more fd
// opened first, so that both even and odd pipe fds are used
foreach ([0, 1] as $extra) {
    $pm = new SwooleTest\ProcessManager;
    $pm->parentFunc = function () use ($pm) {
        $ok = 0;
        co::run(function () use ($pm, &$ok) {
            for ($i = 0; $i < 8; $i++) {
                $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
                $cli->set(['timeout' => 3]);
                if ($cli->get('/') && $cli->statusCode === 200 && $cli->body === 'ok') {
                    $ok++;
                }
            }
        });
        echo "responses: {$ok}/8\n";
        $pm->kill();
    };
    $pm->childFunc = function () use ($pm, $extra) {
        $fds = [];
        for ($i = 0; $i < $extra; $i++) {
            $fds[] = fopen('/dev/null', 'r');
        }
        Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
        $serv = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
        $serv->set([
            'log_file' => '/dev/null',
            'worker_num' => 4,
            'reactor_num' => 2,
        ]);
        $serv->on('workerStart', function ($serv, $id) use ($pm) {
            if ($id === 0) {
                $pm->wakeup();
            }
        });
        $serv->on('request', function ($request, $response) {
            $response->end('ok');
        });
        $serv->start();
    };
    $pm->childFirst();
    $pm->run();
}
?>
--EXPECT--
responses: 8/8
responses: 8/8
