--TEST--
swoole_server: bug_11000_01
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use Swoole\Server;

$pm = new SwooleTest\ProcessManager;

$pm->childFunc = function () {
    $port = get_one_free_port();
    $serv = new Server(TCP_SERVER_HOST, $port);
    $process = new \OpenSwoole\Process(function ($process) use ($serv) {
        usleep(10000);
        var_dump($serv->stats()['up']);
        $serv->shutdown();
    });
    $serv->set(['worker_num' => 2, 'log_file' => '/dev/null']);
    $serv->on('receive', function () { });
    $serv->addProcess($process);
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
int(1)
