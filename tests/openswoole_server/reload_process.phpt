--TEST--
openswoole_server: reload in process mode
--CONFLICTS--
all
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
//skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$worker_num = $task_worker_num = OpenSwoole\Util::getCPUNum() * 2;
$counter = [
    'worker' => new OpenSwoole\Atomic(),
    'task_worker' => new OpenSwoole\Atomic()
];
$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    global $counter, $worker_num, $task_worker_num;
    while (!file_exists(TEST_PID_FILE)) {
        usleep(100 * 1000);
    }
    $pid = (int) file_get_contents(TEST_PID_FILE);
    $random = mt_rand(1, 4);
    usleep(100 * 1000);
    for ($n = $random; $n--;) {
        OpenSwoole\Process::kill($pid, SIGUSR1);
        usleep(500 * 1000);
        OpenSwoole\Process::kill($pid, SIGUSR2);
        usleep(500 * 1000);
    }

    /**@var $counter OpenSwoole\Atomic[] */
    $total = $counter['worker']->get() - $worker_num;
    $expect = $random * $worker_num;
    Assert::same($total, $expect, "[worker reload {$total} but expect {$expect}]");

    $total = $counter['task_worker']->get() - $task_worker_num;
    $expect = $random * $task_worker_num * 2;
    Assert::same($total, $expect, "[task worker reload {$total} but expect {$expect}]");

    $log = file_get_contents(TEST_LOG_FILE);
    $log = trim(preg_replace('/.+?\s+?INFO\s+?.+/', '', $log));
    if (!Assert::assert(empty($log))){
        var_dump($log);
    }
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    global $worker_num, $task_worker_num;
    @unlink(TEST_LOG_FILE);
    @unlink(TEST_PID_FILE);
    $server = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => TEST_LOG_FILE,
        'pid_file' => TEST_PID_FILE,
        'worker_num' => $worker_num,
        'task_worker_num' => $task_worker_num
    ]);
    $server->on('ManagerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('WorkerStart', function (OpenSwoole\Server $server, int $worker_id) use ($pm) {
        /**@var $counter OpenSwoole\Atomic[] */
        global $counter;
        $atomic = $server->taskworker ? $counter['task_worker'] : $counter['worker'];
        $atomic->add(1);
    });
    $server->on('Receive', function (OpenSwoole\Server $server, $fd, $reactor_id, $data) { });
    $server->on('Task', function () { });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
