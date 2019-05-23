--TEST--
swoole_coroutine/scheduler: for
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php
require __DIR__ . '/../../include/bootstrap.php';

$max_msec = 10;
ini_set("swoole.enable_preemptive_scheduler","1");
$default = 10;
$start = microtime(1);
echo "start\n";
$flag = 1;

go(function () use (&$flag) {
    echo "coro 1 start to loop\n";
    $i = 0;
    for (; ;) {
        if (!$flag) {
            break;
        }
        $i++;
    }
    echo "coro 1 can exit\n";
});

$end = microtime(1);
$msec = ($end - $start) * 1000;
USE_VALGRIND || Assert::lessThanEq(abs($msec - $max_msec), $default);

go(function () use (&$flag) {
    echo "coro 2 set flag = false\n";
    $flag = false;
});
echo "end\n";
?>
--EXPECTF--
start
coro 1 start to loop
coro 2 set flag = false
end
coro 1 can exit
