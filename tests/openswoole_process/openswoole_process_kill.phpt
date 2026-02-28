--TEST--
openswoole_process: kill
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function() {
    sleep(PHP_INT_MAX);
});
$pid = $proc->start();
openswoole_process::kill($pid, SIGKILL);
$i = \openswoole_process::wait(true);
Assert::same($i["signal"], SIGKILL);
echo "SUCCESS";
?>
--EXPECT--
SUCCESS
