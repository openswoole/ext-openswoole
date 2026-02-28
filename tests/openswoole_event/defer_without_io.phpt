--TEST--
openswoole_event: openswoole_event_defer without io
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

openswoole_event_defer(function () {
    echo "defer [1]\n";
});

openswoole_event_wait();
?>
--EXPECT--
defer [1]
