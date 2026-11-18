--TEST--
openswoole_socket_coro: getpeername
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

co::run(
    function () use ($server) {
        $conn = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($conn->connect($server->host, $server->httpPort));
        $info = $conn->getpeername();
        Assert::eq($info['address'], '127.0.0.1');
        Assert::eq($info['port'], $server->httpPort);
    }
);
?>
--EXPECT--
