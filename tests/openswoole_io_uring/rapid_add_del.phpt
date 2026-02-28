--TEST--
openswoole_io_uring: rapid add then immediate del exercises ECANCELED path
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

$pairs = [];
for ($i = 0; $i < 5; $i++) {
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    stream_set_blocking($pair[0], false);

    // Add then immediately del without any event firing
    openswoole_event_add($pair[0], function ($fp) {
        echo "should not fire\n";
    });
    openswoole_event_del($pair[0]);

    $pairs[] = $pair;
}

// Add one more fd that actually fires to verify reactor still works
$final = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
stream_set_blocking($final[0], false);
openswoole_event_add($final[0], function ($fp) {
    $data = fread($fp, 8192);
    echo "ok: $data\n";
    openswoole_event_del($fp);
    fclose($fp);
});

fwrite($final[1], "still works");
fclose($final[1]);

\OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
ok: still works
DONE
