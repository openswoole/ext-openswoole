--TEST--
swoole_io_uring: timer with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$count = 0;
$id = swoole_timer_tick(10, function () use (&$count, &$id) {
    $count++;
    echo "tick $count\n";
    if ($count >= 3) {
        swoole_timer_clear($id);
    }
});

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
tick 1
tick 2
tick 3
DONE
