--TEST--
swoole_coroutine_scheduler/preemptive: swoole_coroutine_scheduler/disable2
--SKIPIF--
<?php 
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$default = 30;
$max_msec = 10;
co::set(['enable_preemptive_scheduler' => true]);

$start = microtime(true);
echo "start\n";
$flag = 1;

go(function () use (&$flag, $max_msec, $start) {
    echo "coro 1 start to loop\n";
    $i = 0;
    while ($flag) {
        $i++;
    }
    echo "coro 1 can exit\n";
});

$end = microtime(true);
$msec = ($end - $start) * 1000;

go(function () use (&$flag) {
    echo "coro 2 set flag = false\n";
    $flag = false;
});
echo "end\n";
swoole_event::wait();
?>
--EXPECTF--
start
coro 1 start to loop
coro 2 set flag = false
end
coro 1 can exit
