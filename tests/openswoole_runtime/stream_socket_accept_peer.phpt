--TEST--
openswoole_runtime: stream_socket_accept() returns the peer address
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
co::run(function () {
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    $address = stream_socket_get_name($server, false);
    go(function () use ($address) {
        $client = stream_socket_client("tcp://{$address}");
        $local = stream_socket_get_name($client, false);
        co::usleep(100000);
        fwrite($client, $local);
    });
    $conn = stream_socket_accept($server, 5, $peer);
    $local = fread($conn, 64);
    var_dump($peer === $local);
});
?>
--EXPECT--
bool(true)
