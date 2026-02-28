--TEST--
openswoole_process: start
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function() {
    echo "SUCCESS";
});
$r = $proc->start();
Assert::assert($r > 0);
$proc->close();

\openswoole_process::wait(true);
?>
--EXPECT--
SUCCESS
