--TEST--
openswoole_server/task: SIGPIPE is ignored in task workers
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Process;
use OpenSwoole\Server;

const LOG_FILE = __DIR__ . '/sigpipe.log';
@unlink(LOG_FILE);

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $serv->set([
        'worker_num' => 1,
        'task_worker_num' => 1,
        'log_file' => LOG_FILE,
    ]);
    $serv->on('workerStart', function (Server $serv, int $id) {
        if (!$serv->taskworker) {
            $serv->task('pipe');
        }
    });
    $serv->on('task', function (Server $serv, $taskId, $workerId, $data) {
        // what a write to a closed socket raises
        Process::kill(getmypid(), SIGPIPE);
        usleep(100000);
        return 'alive';
    });
    $serv->on('finish', function (Server $serv, $taskId, $data) use ($pm) {
        echo "task worker: {$data}\n";
        $pm->wakeup();
    });
    $serv->on('receive', function () {});
    $serv->start();
};
$pm->childFirst();
$pm->run();
$log = (string) @file_get_contents(LOG_FILE);
echo strpos($log, 'Broken pipe') === false ? "no warning\n" : $log;
@unlink(LOG_FILE);
?>
--EXPECT--
task worker: alive
no warning
