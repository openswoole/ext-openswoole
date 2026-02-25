--TEST--
swoole_io_uring: multiple file descriptors with io_uring backend
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$results = [];
$num_pairs = 4;
$write_ends = [];

for ($i = 0; $i < $num_pairs; $i++) {
    $pair = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
    stream_set_blocking($pair[0], false);
    stream_set_blocking($pair[1], false);

    $idx = $i;
    swoole_event_add($pair[0], function ($fp) use (&$results, $idx) {
        $data = fread($fp, 8192);
        $results[$idx] = $data;
        swoole_event_del($fp);
        fclose($fp);
    });

    $write_ends[$i] = $pair[1];
}

// Write to all pairs
for ($i = 0; $i < $num_pairs; $i++) {
    fwrite($write_ends[$i], "msg-$i");
    fclose($write_ends[$i]);
}

\OpenSwoole\Event::wait();

ksort($results);
foreach ($results as $i => $msg) {
    echo "$msg\n";
}
echo "count: " . count($results) . "\n";
echo "DONE\n";
?>
--EXPECT--
msg-0
msg-1
msg-2
msg-3
count: 4
DONE
