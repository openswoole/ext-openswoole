--TEST--
openswoole_process: pipe read timeout
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $process) {
    sleep(5);
});
$r = $proc->start();
Assert::assert($r > 0);
ini_set("openswoole.display_errors", "off");
$proc->setTimeout(0.5);
$ret = $proc->read();
Assert::false($ret);
openswoole_process::kill($proc->pid, SIGKILL);
\openswoole_process::wait(true);
?>
--EXPECT--
