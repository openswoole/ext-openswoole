--TEST--
swoole_coroutine_scheduler/preemptive: while with opcache enable
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

co::set(['enable_preemptive_scheduler' => true]);

Co\run(function() {
    $maxspan = 30;
    $start = microtime(true);
    echo "start\n";
    $flag = 1;
    go(function () use (&$flag) {
        echo "coro 1 start to loop\n";
        $i = 0;
        while ($flag) {
            $i++;
        }
        echo "coro 1 can exit\n";
    });
    $end = microtime(true);
    $timespan = ($end - $start) * 1000;
    USE_VALGRIND || Assert::lessThanEq($timespan, $maxspan);
    go(function () use (&$flag) {
        echo "coro 2 set flag = false\n";
        $flag = false;
    });
    echo "end\n";
});
?>
--EXPECTF--
start
coro 1 start to loop
coro 2 set flag = false
end
coro 1 can exit
