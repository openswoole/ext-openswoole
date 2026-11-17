--TEST--
openswoole_server: bug_11000_01
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Server;

$pm = new SwooleTest\ProcessManager;
// the server shuts itself down, the parent only waits for it
$pm->parentFunc = function () {
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort());
    $process = new \OpenSwoole\Process(function ($process) use ($serv) {
        usleep(10000);
        var_dump($serv->stats()['up']);
        $serv->shutdown();
    });
    $serv->set(['worker_num' => 2, 'log_file' => '/dev/null']);
    $serv->on('start', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function () { });
    $serv->addProcess($process);
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
int(1)
