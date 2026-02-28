--TEST--
openswoole_event: dispatch
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$id = openswoole_timer_tick(100, function () {
    echo "Tick\n";
});

$n = 5;
while ($n--) {
    echo "loop\n";
    openswoole_event_dispatch();
}

openswoole_timer_clear($id);

?>
--EXPECT--
loop
loop
Tick
loop
Tick
loop
Tick
loop
Tick
