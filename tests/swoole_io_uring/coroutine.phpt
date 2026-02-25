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
Co\run(function () {
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    $read_end = $pair[0];
    $write_end = $pair[1];

    go(function () use ($read_end) {
        $data = Co::readSocket($read_end);
        echo "coroutine received: $data\n";
        fclose($read_end);
    });

    go(function () use ($write_end) {
        Co::writeSocket($write_end, "hello coroutine");
        fclose($write_end);
    });
});

echo "DONE\n";
?>
--EXPECT--
coroutine received: hello coroutine
DONE
