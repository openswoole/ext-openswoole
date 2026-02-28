--TEST--
openswoole_process: signal in task worker
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Process;
use OpenSwoole\Server;
use OpenSwoole\Coroutine\Client;


define('PID_FILE', __DIR__.'/task_worker.pid');

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {
    $_pid = file_get_contents(PID_FILE);
    Process::kill((int)$_pid, SIGINT);
    $pm->wait();
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort());
    $serv->set([
        'worker_num' => 1,
        'task_worker_num' => 1,
        'log_file' => '/dev/null'
    ]);

    $serv->on("WorkerStart", function (Server $serv) use ($pm) {
        if ($serv->taskworker) {
            file_put_contents(PID_FILE, $serv->getWorkerPid());
            Process::signal(SIGINT, function () use($pm) {
                echo "SIGINT triggered\n";
                sleep(1);
                $pm->wakeup();
            });
            $pm->wakeup();
        }
    });
    $serv->on("Task", function (Server $serv) use ($pm) {

    });
    $serv->on("Receive", function (Server $serv, $fd, $reactorId, $data) {
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
unlink(PID_FILE);
?>
--EXPECT--
SIGINT triggered
