--TEST--
openswoole_event: openswoole_event_defer
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

openswoole_event_defer(function () {
    echo "defer [1]\n";
});
openswoole_timer_after(100, function () {
    echo "timer [1]\n";
    openswoole_timer_after(100, function () {
        echo "timer [2]\n";
    });
    openswoole_event_defer(function () {
        echo "defer [2]\n";
    });
});
openswoole_event_wait();
?>
--EXPECT--
defer [1]
timer [1]
defer [2]
timer [2]
