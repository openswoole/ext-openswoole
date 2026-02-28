--TEST--
openswoole_process: freeQueue
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function() {});
$r  = $proc->useQueue();
Assert::assert($r);

$proc->start();
$r  = $proc->freeQueue();
Assert::assert($r);

\openswoole_process::wait();

?>
--EXPECT--
