--TEST--
openswoole_runtime: accept timeout
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$port = 0;

swoole\runtime::enableCoroutine();

go(function () use (&$port) {
    $opts = array(
        'socket' => array(
            'so_reuseaddr' => true,
        ),
    );
    $ctx = stream_context_create($opts);
    $socket = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN, $ctx);
    $port = $socket ? (int) explode(':', stream_socket_get_name($socket, false))[1] : 0;
    if (!$socket) {
        echo "$errstr ($errno)<br />\n";
    } else {
        $conn = @stream_socket_accept($socket, 1);
        Assert::false($conn);
    }
});

openswoole_event_wait();
?>
--EXPECT--
