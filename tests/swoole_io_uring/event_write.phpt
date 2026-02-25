--TEST--
swoole_io_uring: event write with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$read_end = $pair[0];
$write_end = $pair[1];

stream_set_blocking($read_end, false);
stream_set_blocking($write_end, false);

swoole_event_add($read_end, function ($fp) {
    $data = fread($fp, 8192);
    echo "received: $data\n";
    swoole_event_del($fp);
    fclose($fp);
});

swoole_event_write($write_end, "hello from event_write");
echo "Finish\n";
\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
Finish
received: hello from event_write
DONE
