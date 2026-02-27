--TEST--
openswoole_event: openswoole_event_exit
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

openswoole_timer_tick(1, function() {
    echo "tick\n";
    openswoole_event_exit();
});
OpenSwoole\Event::wait();
?>
--EXPECT--
tick
