--TEST--
swoole_coroutine_scheduler/preemptive: swoole_coroutine_scheduler/disable
--SKIPIF--
<?php 
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$default = 10;
$max_msec = 10;

co::set([
    'enable_preemptive_scheduler' => true, 
    'hook_flags' => 0,
]);

$start = microtime(true);
echo "start\n";
$flag = 1;

go(function () use (&$flag, $max_msec, $start) {
    Swoole\Coroutine::disableScheduler();
    echo "coro 1 start to loop\n";
    $i = 0;
    while ($flag) {
        $i++;
        $m = microtime(true);
        usleep(500000);
        if (($m-$start) * 1000 > 500) {
            echo "coro 1 exec more 500ms and break\n";
            break;
        }
    }
    echo "coro 1 can exit\n";
    Swoole\Coroutine::enableScheduler();
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
coro 1 exec more 500ms and break
coro 1 can exit
coro 2 set flag = false
end
