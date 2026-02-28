--TEST--
openswoole_process/coro: start with coroutine
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$proc = new \openswoole_process(function () {
    co::usleep(200000);
    echo "SUCCESS\n";
}, false, 1, true);

$r = $proc->start();
Assert::assert($r > 0);
$proc->close();

\openswoole_process::wait(true);

?>
--EXPECT--
SUCCESS
