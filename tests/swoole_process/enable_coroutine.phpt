--TEST--
swoole_process: push
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$process = new OpenSwoole\Process(function(Swoole\Process $worker) {
    echo Co::getCid() . PHP_EOL;
}, false, 0, true);
$process->start();
$process::wait();

$process = new OpenSwoole\Process(function(Swoole\Process $worker) {
    echo Co::getCid() . PHP_EOL;
}, false, 0, false);
$process->set(['enable_coroutine' => true]);
$process->start();
$process::wait();

?>
--EXPECT--
1
1
