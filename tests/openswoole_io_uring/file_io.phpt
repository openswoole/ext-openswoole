--TEST--
openswoole_io_uring: coroutine socket I/O with io_uring reactor
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    // TCP socket I/O via io_uring reactor
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    $read_sock = $pair[0];
    $write_sock = $pair[1];
    stream_set_blocking($read_sock, false);
    stream_set_blocking($write_sock, false);

    $data_sent = "hello io_uring reactor";

    go(function () use ($write_sock, $data_sent) {
        Co::sleep(1);
        fwrite($write_sock, $data_sent);
        fclose($write_sock);
    });

    go(function () use ($read_sock, $data_sent) {
        $content = '';
        while (!feof($read_sock)) {
            $chunk = @fread($read_sock, 8192);
            if ($chunk === false || $chunk === '') {
                Co::sleep(1);
                continue;
            }
            $content .= $chunk;
        }
        fclose($read_sock);
        Assert::same($content, $data_sent);
        echo "read: $content\n";
    });
});

echo "DONE\n";
?>
--EXPECT--
read: hello io_uring reactor
DONE
