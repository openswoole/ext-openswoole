--TEST--
openswoole_runtime/stream_select: rw events
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$port = 0;

OpenSwoole\Runtime::enableCoroutine();

$n = new openswoole_atomic(1);

go(function () use (&$port, $n) {
    $server = stream_socket_server("tcp://127.0.0.1:0", $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN);
    $port = $server ? (int) explode(':', stream_socket_get_name($server, false))[1] : 0;
    while($n->get()) {
        $conn = @stream_socket_accept($server, 0.1);
        if ($conn) {
            go(function () use (&$port, $conn) {
                fwrite($conn, 'The local time is ' . date('n/j/Y g:i a'));
                echo fread($conn, 8192);
                fclose($conn);
            });
        }
    }
});

go(function () use (&$port, $n) {
    $fp1 = stream_socket_client("tcp://127.0.0.1:{$port}", $errno, $errstr, 30);
    $fp2 = stream_socket_client("tcp://127.0.0.1:{$port}", $errno, $errstr, 30);
    $r_array = [$fp1, $fp2];
    $w_array = [$fp1, $fp2];
    $e_array = null;
    $retval = stream_select($r_array, $w_array, $e_array, 10);
    Assert::same($retval, 2);
    Assert::same(count($r_array), 2);
    Assert::same(count($w_array), 2);
    $n->set(0);
});
?>
--EXPECT--
