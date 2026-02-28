--TEST--
openswoole_io_uring: openswoole_event_set called from within event handler
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

$call_count = 0;

openswoole_event_add($read_end, function ($fp) use (&$call_count) {
    $call_count++;
    $data = fread($fp, 8192);

    if ($call_count === 1) {
        echo "first: $data\n";
        // Modify callback from within handler (triggers cancel + re-add)
        openswoole_event_set($fp, function ($fp) use (&$call_count) {
            $call_count++;
            $data = fread($fp, 8192);
            echo "second: $data\n";
            openswoole_event_del($fp);
            fclose($fp);
        }, null, SWOOLE_EVENT_READ);
    }
});

// Write first message
fwrite($write_end, "msg1");

// Defer the second write so it happens after the first callback
openswoole_event_defer(function () use ($write_end) {
    fwrite($write_end, "msg2");
    fclose($write_end);
});

\OpenSwoole\Event::wait();
Assert::same($call_count, 2);
echo "DONE\n";
?>
--EXPECT--
first: msg1
second: msg2
DONE
