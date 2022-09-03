--TEST--
swoole_process: null callback
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$process = new OpenSwoole\Process(function () { });
$p = new ReflectionProperty($process, 'callback');
$p->setAccessible(true);
$p->setValue($process, null);

$pid = $process->start();
usleep(100);
$process->wait();
?>
--EXPECTF--
Fatal error: OpenSwoole\Process::start(): Illegal callback function of OpenSwoole\Process in %s
