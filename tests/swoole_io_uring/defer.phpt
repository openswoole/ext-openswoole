--TEST--
swoole_io_uring: defer with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

swoole_event_defer(function () {
    echo "defer 1\n";
    swoole_event_defer(function () {
        echo "defer 2\n";
    });
});

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
defer 1
defer 2
DONE
