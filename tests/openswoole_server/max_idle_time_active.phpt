--TEST--
openswoole_server: max_idle_time keeps active connections and closes idle ones
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        // sends every 300ms for 3s, longer than max_idle_time
        go(function () use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            for ($i = 0; $i < 10; $i++) {
                Assert::assert($cli->send("ping\n"));
                Assert::same($cli->recv(2), "pong\n");
                co::usleep(300000);
            }
            echo "active: open\n";
        });
        // sends nothing, closed after about max_idle_time
        go(function () use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $start = microtime(true);
            Assert::same($cli->recv(10), '');
            $elapsed = microtime(true) - $start;
            Assert::greaterThanEq($elapsed, 0.9);
            Assert::lessThan($elapsed, 2.5);
            echo "idle: closed\n";
        });
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'max_idle_time' => 1,
    ]);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('receive', function ($server, $fd, $rid, $data) {
        $server->send($fd, "pong\n");
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
idle: closed
active: open
