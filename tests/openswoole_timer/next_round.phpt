--TEST--
openswoole_timer: timer round control
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Timer;
use OpenSwoole\Event;

Timer::after(10, function () {
    Assert::eq(timer::stats()['round'], 1);
    Timer::after(10, function () {
        Assert::eq(timer::stats()['round'], 2);
    });
    usleep(100000);
});

Event::wait();
?>
--EXPECT--
