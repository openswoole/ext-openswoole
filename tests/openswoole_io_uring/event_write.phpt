--TEST--
openswoole_io_uring: openswoole_event_write with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
$pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$read_end = $pair[0];
$write_end = $pair[1];

stream_set_blocking($read_end, false);
stream_set_blocking($write_end, false);

// Monitor read end for incoming data
openswoole_event_add($read_end, function ($fp) {
    $data = fread($fp, 8192);
    echo "received: $data\n";
    openswoole_event_del($fp);
    fclose($fp);
});

// Monitor write end; when writable, use openswoole_event_write to send data
openswoole_event_add($write_end, null, function ($fp) {
    openswoole_event_write($fp, "hello from event_write");
    openswoole_event_del($fp);
    fclose($fp);
}, SWOOLE_EVENT_WRITE);

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
received: hello from event_write
DONE
