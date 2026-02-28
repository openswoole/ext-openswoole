--TEST--
openswoole_process: wait
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function() {});
$pid = $proc->start();
$info = \openswoole_process::wait(true);
Assert::same($pid, $info["pid"]);
Assert::same($info["code"], 0);
Assert::same($info["signal"], 0);

$proc = new \openswoole_process(function() { exit(1); });
$pid = $proc->start();
$info = \openswoole_process::wait(true);
Assert::same($pid, $info["pid"]);
Assert::same($info["code"], 1);
Assert::same($info["signal"], 0);

$proc = new \openswoole_process(function() { \openswoole_process::kill(posix_getpid(), SIGTERM); });
$pid = $proc->start();
$info = \openswoole_process::wait(true);
Assert::same($pid, $info["pid"]);
Assert::same($info["code"], 0);
Assert::same($info["signal"], SIGTERM);

echo "SUCCESS";
?>
--EXPECT--
SUCCESS
