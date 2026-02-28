--TEST--
openswoole_coroutine: call_not_exists_func
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function (int $pid) use ($pm) {
    go(function () use ($pm) {
        Assert::throws(function () use ($pm) {
            httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        }, Exception::class);
        $pm->kill();
    });
};
$pm->childFunc = function () use ($pm) {
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['worker_num' => 1]);
    $http->on('workerStart', function () use ($pm) {
        none();
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) {
        Co::usleep(1000);
        throw new Exception('whoops');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function none() in %s:%d
Stack trace:
%A
  thrown in %s on line %d
[%s]	ERROR	php_openswoole_server_rshutdown() (ERRNO %d): Fatal error: Uncaught Error: Call to undefined function none() in %s:%d
Stack trace:
%A
  thrown in %s on line %d
