--TEST--
openswoole_process_pool: getProcess [3]
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Process\Pool;
use OpenSwoole\Process;

const N = 70000;

$pool = new Pool(2, SWOOLE_IPC_UNIXSOCK);

$pool->on('workerStart', function (OpenSwoole\Process\Pool $pool, int $workerId) {
    if ($workerId == 0) {
        usleep(1000);
        $process1 = $pool->getProcess(1);
        phpt_var_dump($process1);
        $pid1 = $process1->pid;
        $success = Process::kill($process1->pid, SIGKILL);
        Assert::true($success);
        usleep(20000);
        $process2 = $pool->getProcess(1);
        phpt_var_dump($process2);
        $pid2 = $process2->pid;
        Assert::notEq($pid1, $pid2);
        $pool->shutdown();
    }
});

$pool->on("message", function ($pool, $data) {

});

$pool->start();
?>
--EXPECTF--
[%s]	WARNING	ProcessPool::wait(): worker#1 abnormal exit, status=0, signal=9
