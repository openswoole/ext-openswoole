--TEST--
swoole_process_pool: getProcess [1]
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
if (function_exists('posix_getpid') == false) {
    die("SKIP, no posix extension.");
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pool = new OpenSwoole\Process\Pool(1);
$pool->on('workerStart', function (Swoole\Process\Pool $pool, int $workerId)
{
    $process = $pool->getProcess();
    Assert::same($process->pid, posix_getpid());
    $pool->shutdown();
    sleep(20);
    echo "ERROR\n";
});

$pool->start();
?>
--EXPECT--
