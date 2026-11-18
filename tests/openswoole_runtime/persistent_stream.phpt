--TEST--
openswoole_runtime: persistent sockets opened in a coroutine are freed correctly
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);

co::run(function () {
    $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
    $port = (int) explode(':', stream_socket_get_name($server, false))[1];
    go(function () use ($server) {
        for ($i = 0; $i < 3; $i++) {
            if ($conn = @stream_socket_accept($server, 5)) {
                fclose($conn);
            }
        }
    });
    for ($i = 0; $i < 3; $i++) {
        // a persistent id makes PHP keep the stream path in persistent memory
        $fp = pfsockopen('127.0.0.1', $port, $errno, $errstr, 5);
        Assert::notEmpty($fp);
        fclose($fp);
    }
    echo "pfsockopen closed\n";
});
?>
--EXPECT--
pfsockopen closed
