--TEST--
swoole_server: register sigint handler with base mode
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    usleep(10000);
    $result = Swoole\Process::kill($pid, SIGINT);
    Assert::true($result);
    usleep(30000);
    echo file_get_contents(TEST_LOG_FILE);
};
$pm->childFunc = function () use ($pm) {
    $server = new Swoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
    ]);
    $server->on('workerStart', function (Swoole\Server $server) use ($pm) {
        \Swoole\Process::signal(2, function () use ($server) {
            file_put_contents(TEST_LOG_FILE, 'SIGINT, SHUTDOWN' . PHP_EOL);
            $server->shutdown();
        });
        $pm->wakeup();
    });
    $server->on('Receive', function (Swoole\Server $server, $fd, $reactorId, $data) {
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
@unlink(TEST_LOG_FILE);
?>
--EXPECT--
SIGINT, SHUTDOWN
