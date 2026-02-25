--TEST--
swoole_io_uring: coroutine with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
Co::run(function () {
    $chan = new OpenSwoole\Coroutine\Channel(1);

    go(function () use ($chan) {
        $chan->push("hello coroutine");
    });

    go(function () use ($chan) {
        $data = $chan->pop();
        echo "coroutine received: $data\n";
    });
});

echo "DONE\n";
?>
--EXPECT--
coroutine received: hello coroutine
DONE
