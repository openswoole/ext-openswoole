--TEST--
openswoole_process_pool: enable coroutine
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pool = new OpenSwoole\Process\Pool(1, SWOOLE_IPC_NONE, 0, true);
$counter = new OpenSwoole\Atomic(0);

$pool->on('workerStart', function (OpenSwoole\Process\Pool $pool, int $workerId) use ($counter) {
    $n = $counter->add(1);
    // the manager may start another worker while the shutdown is in progress
    if ($n > 6) {
        return;
    }
    co::usleep(50000);
    echo "hello world\n";
});

$pool->on("workerStop", function ($pool, $data) use ($counter) {
    $n = $counter->get();
    if ($n <= 6) {
        echo "worker stop\n";
    }
    if ($n >= 6) {
        $pool->shutdown();
    }
});

$pool->start();
?>
--EXPECT--
hello world
worker stop
hello world
worker stop
hello world
worker stop
hello world
worker stop
hello world
worker stop
hello world
worker stop
