--TEST--
openswoole_server: bug Github#2313
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Server('127.0.0.1', 9501);
    $process = new OpenSwoole\Process(function () { });
    $server->addProcess($process);
    var_dump($process->id);
    $pm->wakeup();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
int(%d)
