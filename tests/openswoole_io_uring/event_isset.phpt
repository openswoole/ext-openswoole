--TEST--
openswoole_io_uring: openswoole_event_isset with io_uring backend
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

openswoole_event_add($pair[0], function ($fp) {
    openswoole_event_del($fp);
    fclose($fp);
});

Assert::true(openswoole_event_isset($pair[0], SWOOLE_EVENT_READ));
Assert::false(openswoole_event_isset($pair[0], SWOOLE_EVENT_WRITE));
echo "isset checks passed\n";

openswoole_event_del($pair[0]);
Assert::false(openswoole_event_isset($pair[0]));
echo "del check passed\n";

fclose($pair[0]);
fclose($pair[1]);
echo "DONE\n";
?>
--EXPECT--
isset checks passed
del check passed
DONE
