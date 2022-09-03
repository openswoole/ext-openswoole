--TEST--
swoole_http_server: max_request_execution_time
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->initRandomData(2);
$pm->parentFunc = function ($pid) use ($pm) {
    switch_process();
    go(function () use ($pm) {
        $statusCode = httpGetStatusCode("http://127.0.0.1:{$pm->getFreePort()}", ['timeout' => 3]);
        echo PHP_EOL . $statusCode . PHP_EOL;
        $pm->kill();
    });
};
$pm->childFunc = function () use ($pm) {
    Co::set(['hook_flags' => SWOOLE_HOOK_ALL]);
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $http->set(['enable_coroutine' => true, 'max_request_execution_time' => 1, 'max_wait_time' => 1]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (Swoole\Http\Request $request, Swoole\Http\Response $response) use ($pm) {
        sleep(20);
        echo "never here\n";
        $response->end($pm->getRandomData());
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
408
[%s]	INFO	Server is shutdown now
[%s]	WARNING	%s (ERRNO 9012): worker exit timeout, forced termination
