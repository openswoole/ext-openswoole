--TEST--
swoole_io_uring: event set to change callbacks with io_uring backend
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

function new_read_callback($fp) {
    $data = fread($fp, 8192);
    swoole_event_del($fp);
    fclose($fp);
    echo "new_callback: $data\n";
}

swoole_event_add($read_end, function ($fp) {
    echo "original callback should not fire\n";
});

// Replace the read callback
swoole_event_set($read_end, 'new_read_callback');

fwrite($write_end, "test data");
fclose($write_end);

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
new_callback: test data
DONE
