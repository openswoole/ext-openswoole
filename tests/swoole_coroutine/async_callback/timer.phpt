--TEST--
swoole_coroutine/async_callback: timer
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Timer;

$GLOBALS['count'] = 0;

co::run(function () {
    Timer::tick(50, function ($timer) {
        $GLOBALS['count']++;
        if ($GLOBALS['count'] == 5) {
            Timer::clear($timer);
        }
        co::usleep(500000);
        echo "tick\n";
    });
});
?>
--EXPECT--
tick
tick
tick
tick
tick
