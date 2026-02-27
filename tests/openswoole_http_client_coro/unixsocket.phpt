--TEST--
openswoole_http_client_coro: http unix-socket
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    for ($c = MAX_CONCURRENCY; $c--;) {
        go(function () use ($pm) {
            $client = new OpenSwoole\Coroutine\Http\Client('unix:' . str_repeat('/', mt_rand(0, 2)) . UNIXSOCK_PATH);
            for ($n = MAX_REQUESTS; $n--;) {
                Assert::assert($client->get('/'), "statusCode={$client->statusCode}, error={$client->errCode}");
                Assert::same($client->body, 'Hello Swoole!');
            }
        });
    }
    openswoole_event_wait();
    echo "SUCCESS\n";
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Http\Server(UNIXSOCK_PATH, 0, SERVER_MODE_RANDOM, SWOOLE_UNIX_STREAM);
    $server->set(['log_file' => '/dev/null']);
    $server->on(\OpenSwoole\Constant::EVENT_START, function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $response->end('Hello Swoole!');
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
