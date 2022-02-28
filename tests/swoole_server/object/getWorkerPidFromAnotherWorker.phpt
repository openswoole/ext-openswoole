--TEST--
swoole_server/object: getWorkerPidFromAnotherWorker()
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        $json = json_decode(httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/"));
        Assert::assert($json->result);
        $pm->kill();
    });
    Swoole\Event::wait();
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new Swoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $serv->set(array(
        'log_level' => SWOOLE_LOG_ERROR,
        'worker_num' => 2,
    ));
    $serv->on("WorkerStart", function (Swoole\Server $serv, $workerId) use ($pm) {
        $pm->wakeup();
        $GLOBALS['pid_worker_'.$workerId] = posix_getpid();
    });
    $serv->on('Request', function ($req, $resp) use ($serv) {
        $resp->end(json_encode(['result' =>
            $GLOBALS['pid_worker_'.$serv->worker_id] == $serv->getWorkerPid($serv->worker_id) &&
            $serv->getWorkerPid(0) != $serv->getWorkerPid(1)]
        ));
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
