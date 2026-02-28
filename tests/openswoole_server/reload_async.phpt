--TEST--
openswoole_server: reload async
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$reloaded = new OpenSwoole\Atomic;
$workerCounter = new OpenSwoole\Atomic;

$pm = new SwooleTest\ProcessManager;
$pm->setWaitTimeout(-1);
$pm->parentFunc = function () use ($pm) {
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm, $reloaded, $workerCounter) {
    $server = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => '/dev/null',
        'worker_num' => rand(2, OpenSwoole\Util::getCPUNum() * 2),
        'max_wait_time' => 10,
        'reload_async' => true,
        'enable_coroutine' => false,
    ]);
    $server->on('WorkerStart', function (OpenSwoole\Server $server, int $worker_id) use ($pm, $reloaded, $workerCounter) {
        $workerCounter->add(1);
        if ($worker_id === 0 and $reloaded->get() != 1) {
            $reloaded->set(1);
            while ($workerCounter->get() < $server->setting['worker_num']) {
                usleep(10000);
            }
            go(function () use ($pm) {
                for ($n = 1; $n <= 5; $n++) {
                    co::usleep(100000);
                    echo "{$n}\n";
                }
                echo "RELOADED\n";
                $pm->wakeup();
            });
            echo "RELOAD\n";
            Assert::assert($server->reload());
        }
    });
    $server->on('Receive', function () { });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
RELOAD
1
2
3
4
5
RELOADED
DONE
