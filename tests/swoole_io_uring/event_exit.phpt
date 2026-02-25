--TEST--
swoole_io_uring: swoole_event_exit with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
swoole_timer_tick(10, function () {
    echo "tick\n";
    swoole_event_exit();
});
\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
tick
DONE
