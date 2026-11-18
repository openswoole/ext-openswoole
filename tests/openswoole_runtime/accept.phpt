--TEST--
openswoole_runtime: accept
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
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
        $conn = stream_socket_accept($socket);
        fwrite($conn, 'The local time is ' . date('n/j/Y g:i a'));
        fclose($conn);
        fclose($socket);
    }
});

go(function () use (&$port) {
    $fp = stream_socket_client("tcp://127.0.0.1:{$port}", $errno, $errstr, 30);
    if (!$fp) {
        echo "$errstr ($errno)<br />\n";
    } else {
        $data = fread($fp, 8192);
        fclose($fp);
        Assert::assert(strpos($data,'local time') !== false);
    }
});

openswoole_event_wait();
?>
--EXPECT--
