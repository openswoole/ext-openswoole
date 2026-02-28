--TEST--
openswoole_io_uring: multiple events on same fd (re-arm / multishot test)
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

$all_data = '';
$event_count = 0;

openswoole_event_add($read_end, function ($fp) use (&$all_data, &$event_count) {
    $data = fread($fp, 8192);
    if ($data === '' || $data === false) {
        openswoole_event_del($fp);
        fclose($fp);
        return;
    }
    $event_count++;
    $all_data .= $data;
});

// Write 3 messages with deferred timing so each triggers a separate event
fwrite($write_end, "one");

openswoole_event_defer(function () use ($write_end) {
    fwrite($write_end, "two");
    openswoole_event_defer(function () use ($write_end) {
        fwrite($write_end, "three");
        fclose($write_end);
    });
});

\OpenSwoole\Event::wait();

// Reads may coalesce, so verify total data and that we got at least 1 event
Assert::true($event_count >= 1, "expected at least 1 read event, got $event_count");
Assert::same($all_data, "onetwothree");
echo "data: $all_data\n";
echo "DONE\n";
?>
--EXPECT--
data: onetwothree
DONE
