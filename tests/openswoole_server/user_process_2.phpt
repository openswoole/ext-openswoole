--TEST--
openswoole_server: user process [with coroutine]
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Server;
$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort());
    $process = new \OpenSwoole\Process(function ($process) use ($serv, $pm) {
        for ($i = 0; $i < 5; $i++) {
            co::usleep(20000);
            echo "$i OK\n";
        }
        $pm->wakeup();
        //kill by SIGTERM
        co::sleep(100);
    }, false, 0, true);
    $serv->set([
        "worker_num" => 1,
        'log_file' => '/dev/null',
    ]);

    $serv->on("Receive", function (Server $serv, $fd, $rid, $data) use ($process) {

    });
    $serv->addProcess($process);
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
0 OK
1 OK
2 OK
3 OK
4 OK
