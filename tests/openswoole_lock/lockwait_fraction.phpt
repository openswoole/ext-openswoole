--TEST--
openswoole_lock: lockwait() keeps the fractional part of the timeout
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_darwin();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$lock = new OpenSwoole\Lock(OPENSWOOLE_MUTEX);
$lock->lock();
$process = new OpenSwoole\Process(function () use ($lock) {
    $start = microtime(true);
    $ret = $lock->lockwait(0.5);
    $elapsed = microtime(true) - $start;
    var_dump($ret);
    var_dump($elapsed > 0.4 && $elapsed < 1.0);
}, false, 0);
$process->start();
OpenSwoole\Process::wait();
$lock->unlock();
?>
--EXPECT--
bool(false)
bool(true)
