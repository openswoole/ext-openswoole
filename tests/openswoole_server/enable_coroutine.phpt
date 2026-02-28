--TEST--
openswoole_server: reload with enable_coroutine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    go(function () use ($pm) {
        for ($i = 2; $i--;) {
            for ($n = 5; $n--;) {
                echo "cid-" . httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/task?n={$n}") . "\n";
            }
            if ($i == 1) {
                OpenSwoole\Process::kill((int)file_get_contents(TEST_PID_FILE), SIGUSR1);
                usleep(100 * 1000);
            }
        }
    });
    OpenSwoole\Event::wait();
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => TEST_LOG_FILE,
        'pid_file' => TEST_PID_FILE,
        'worker_num' => 1,
    ]);
    $server->on('WorkerStart', function (OpenSwoole\Server $server, int $worker_id) use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function (\OpenSwoole\Http\Request $request, \OpenSwoole\Http\Response $response) {
        $response->end(\Co::getCid());
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
cid-2
cid-3
cid-4
cid-5
cid-6
cid-2
cid-3
cid-4
cid-5
cid-6
DONE
