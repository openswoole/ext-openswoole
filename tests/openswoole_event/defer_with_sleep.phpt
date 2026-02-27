--TEST--
openswoole_event: openswoole_event_defer and sleep
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    Co::usleep(1000);
    echo "timer [1]\n";
    openswoole_event_defer(function () {
        echo "defer [2]\n";
        go(function () {
            Co::usleep(1000);
            echo "timer [2]\n";
        });
    });
});
openswoole_event_defer(function () {
    echo "defer [1]\n";
});
openswoole_event_wait();
?>
--EXPECT--
defer [1]
timer [1]
defer [2]
timer [2]
