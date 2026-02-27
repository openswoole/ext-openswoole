--TEST--
openswoole_timer: Timer must be greater than 0
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::false(@openswoole_timer_after(0, function() {}));
Assert::false(@openswoole_timer_after(-1, function() {}));
Assert::false(@openswoole_timer_tick(0, function() {}));
Assert::false(@openswoole_timer_tick(-1, function() {}));
?>
--EXPECTF--
