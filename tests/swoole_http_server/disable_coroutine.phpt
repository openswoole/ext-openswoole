--TEST--
swoole_http_server: disable coroutine and use go
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    go(function () use ($pm) {
        for ($n = 0; $n > MAX_REQUESTS; $n++) {
            Assert::assert(httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/") == $n);
        }
    });
    Swoole\Event::wait();
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $http->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'enable_coroutine' => false, // close build-in coroutine
    ]);
    $http->on("request", function (Swoole\Http\Request $request, Swoole\Http\Response $response) {
        go(function () use ($response) {
            Co::usleep(1000);
            $cid = go(function () use ($response) {
                co::yield();
                $response->end(Co::getuid());
            });
            co::resume($cid);
        });
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
