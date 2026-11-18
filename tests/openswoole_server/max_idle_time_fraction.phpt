--TEST--
openswoole_server: max_idle_time accepts fractions of a second
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $start = microtime(true);
        Assert::same($cli->recv(5), '');
        $elapsed = microtime(true) - $start;
        var_dump($elapsed > 0.3 && $elapsed < 1.5);
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set(['log_file' => '/dev/null', 'worker_num' => 1, 'max_idle_time' => 0.5]);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('receive', function () {});
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
bool(true)
