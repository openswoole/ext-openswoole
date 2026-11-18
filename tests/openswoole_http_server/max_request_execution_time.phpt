--TEST--
openswoole_http_server: max_request_execution_time applies to each request on its own
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();
        $get = function (string $path) use ($port) {
            $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $port);
            $cli->set(['timeout' => 10]);
            $start = microtime(true);
            $cli->get($path);
            return [$cli->statusCode, microtime(true) - $start];
        };
        [$status] = $get('/fast');
        echo "fast: {$status}\n";
        // the timer of /fast would fire 0.5s into /slow if it outlived its request
        co::usleep(1500000);
        [$status, $time] = $get('/slow');
        echo "slow: {$status}, ", $time > 1.8 ? 'after the limit' : sprintf('too early (%.2fs)', $time), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'max_request_execution_time' => 2,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        if ($request->server['request_uri'] === '/slow') {
            co::usleep(3000000);
        }
        $response->end('ok');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
fast: 200
slow: 408, after the limit
