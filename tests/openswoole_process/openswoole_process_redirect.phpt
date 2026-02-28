--TEST--
openswoole_process: redirect
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $proc) {
    echo "SUCCESS";
}, true);

$proc->start();
$r = $proc->read();
echo "READ: $r~";

\openswoole_process::wait(true);
?>
--EXPECT--
READ: SUCCESS~
