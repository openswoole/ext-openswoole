--TEST--
openswoole_http2_server: ping
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        $cli->set(['timeout' => 10]);
        Assert::true($cli->connect());
        Assert::greaterThan($streamId = $cli->send(new OpenSwoole\Http2\Request), 0);
        $s = microtime(true);
        $response = $cli->recv();
        time_approximate(0.5, microtime(true) - $s);
        Assert::same($response->streamId, $streamId);
        $pm->kill();
    });
    OpenSwoole\Event::wait();
};
$pm->childFunc = function () use ($pm) {
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'open_http2_protocol' => true
    ]);
    $http->on('workerStart', function ($serv, $wid) use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) {
        OpenSwoole\Timer::tick(100, function (int $id) use ($request, $response) {
            Assert::true($response->ping());
            if (@++$GLOBALS['i'] === 5) {
                $response->end($request->rawcontent());
                OpenSwoole\Timer::clear($id);
            }
        });
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
