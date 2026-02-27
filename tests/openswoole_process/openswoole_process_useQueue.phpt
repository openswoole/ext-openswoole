--TEST--
openswoole_process: useQueue
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $proc) {
    echo $proc->pop();
});
$proc->useQueue();
$proc->start();
$proc->push("SUCCESS");

\openswoole_process::wait(true);
$proc->freeQueue();
?>
--EXPECT--
SUCCESS
