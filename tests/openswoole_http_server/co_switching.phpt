--TEST--
openswoole_http_server: co switching
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    for ($i = MAX_CONCURRENCY; $i--;) {
        go(function () use ($pm) {
            $cli = new \OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
            for ($i = MAX_REQUESTS; $i--;) {
                Assert::assert($cli->get('/'));
                Assert::same($cli->statusCode, 200);
                Assert::same($cli->body, 'Hello Swoole!');
            }
        });
    }
    openswoole_event_wait();
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new openswoole_http_server('0.0.0.0', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'worker_num' => OpenSwoole\Util::getCPUNum()
    ]);
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($http) {
        go(function () {
            for ($i = 5; $i--;) {
                Co::usleep(1000);
            }
        });
        $response->end('Hello Swoole!');
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
