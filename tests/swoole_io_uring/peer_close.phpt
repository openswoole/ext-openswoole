--TEST--
swoole_io_uring: POLLHUP when peer closes socket
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
$pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
stream_set_blocking($pair[0], false);

swoole_event_add($pair[0], function ($fp) {
    $data = fread($fp, 8192);
    if ($data === '' || $data === false) {
        echo "peer closed\n";
    } else {
        echo "data: $data\n";
    }
    swoole_event_del($fp);
    fclose($fp);
});

// Close peer immediately - this triggers POLLHUP/POLLIN with EOF on read end
fclose($pair[1]);

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
peer closed
DONE
