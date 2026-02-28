--TEST--
openswoole_socket_coro: sendto
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$randomData = '';
co::run(function () {
    go(function () {
        $socket = new OpenSwoole\Coroutine\Socket(AF_UNIX, SOCK_DGRAM, IPPROTO_IP);
        $socket->bind('/tmp/test-server.sock', 0);
        $data = $socket->recvfrom($peer);
        if (!$data) {
            return;
        }
        Assert::same($data, 'hello');
        Assert::same($peer['address'], '/tmp/test-client.sock');
        Assert::same($peer['port'], 0);
        global $randomData;
        for ($n = 0; $n < MAX_CONCURRENCY * MAX_REQUESTS; $n++) {
            $chunk = get_safe_random(1024);
            $randomData .= $chunk;
            Assert::same($socket->sendto($peer['address'], $peer['port'], $chunk), strlen($chunk));
        }
        // close
        Assert::same($socket->sendto($peer['address'], $peer['port'], ''), 0);
    });
    go(function () {
        $socket = new  OpenSwoole\Coroutine\Socket(AF_UNIX, SOCK_DGRAM, IPPROTO_IP);
        $socket->bind('/tmp/test-client.sock', 0);
        $socket->sendto('/tmp/test-server.sock', 0, 'hello');
        $data = '';
        while (true) {
            $tmp = $socket->recvfrom($peer);
            if (empty($tmp)) {
                break;
            }
            $data .= $tmp;
        }
        global $randomData;
        if (Assert::same($data, $randomData)) {
            echo "OK\n";
        }
    });
});
?>
--EXPECT--
OK
