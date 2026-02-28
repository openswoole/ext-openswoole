--TEST--
openswoole_process: exec
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $proc) {
    $proc->exec("/usr/bin/printf", ["HELLO"]);
}, true);
$proc->start();
echo $proc->read();
$proc->exec("/usr/bin/printf", [" WORLD"]);

\openswoole_process::wait(true);
?>
--EXPECT--
HELLO WORLD
