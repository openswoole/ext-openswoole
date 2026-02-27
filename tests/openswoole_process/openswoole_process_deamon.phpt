--TEST--
openswoole_process: deamon
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $proc) {
    $r = \openswoole_process::daemon();
    Assert::assert($r);

    $proc->push((string)posix_getpid());
});
$proc->useQueue();
$forkPid = $proc->start();
$demonPid = intval($proc->pop());

Assert::assert($forkPid !== $demonPid);

\openswoole_process::kill($demonPid, SIGKILL);

\openswoole_process::wait(true);
\openswoole_process::wait(true);
echo "SUCCESS";
?>
--EXPECT--
SUCCESS
