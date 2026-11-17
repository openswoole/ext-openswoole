--TEST--
openswoole_server/object: getWorkerId()
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

const WORKER_N = 4;

$pm = new SwooleTest\ProcessManager;
$started = new OpenSwoole\Atomic(0);

$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        $json = json_decode(httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/"));
        Assert::assert($json->result);
        $pm->kill();
    });
    OpenSwoole\Event::wait();
    $pm->kill();
};

$pm->childFunc = function () use ($pm, $started) {
    $serv = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $serv->set(array(
        'worker_num' => WORKER_N,
        'log_level' => SWOOLE_LOG_ERROR,
    ));
    $serv->on("WorkerStart", function (OpenSwoole\Server $serv, $workerId) use ($pm, $started) {
        $GLOBALS['pid'] = $workerId;
        // send the request once every worker can take it
        if ($started->add() == WORKER_N) {
            $pm->wakeup();
        }
    });
    $serv->on('Request', function ($req, $resp) use ($serv) {
        $resp->end(json_encode(['result' => $GLOBALS['pid'] == $serv->getWorkerId()]));
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
