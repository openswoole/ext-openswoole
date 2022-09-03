--TEST--
swoole_coroutine: new process
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $process = new Swoole\Process(function () { });
    $process->start();
});
?>
--EXPECTF--
Fatal error: Uncaught OpenSwoole\Error: must be forked outside the coroutine in %s:%d
Stack trace:
#0 %s(5): OpenSwoole\Process->start()
%A
  thrown in %s on line %d
