--TEST--
openswoole_server: unregistered signal, ignore SIGPIPE
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
$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    $pid = file_get_contents(TEST_PID_FILE);
    usleep(10000);
    OpenSwoole\Process::kill((int)$pid, SIGPIPE);
    usleep(10000);
    $log = file_get_contents(TEST_LOG_FILE);
    echo $log, "\n";
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    @unlink(TEST_LOG_FILE);
    $server = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => TEST_LOG_FILE,
        'pid_file' => TEST_PID_FILE,
        'worker_num' => 1,
    ]);
    $server->on('WorkerStart', function (OpenSwoole\Server $server, $worker_id) use ($pm) {
        $pm->wakeup();
    });
    $server->on('Receive', function (OpenSwoole\Server $server, $fd, $reactorId, $data) {
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
