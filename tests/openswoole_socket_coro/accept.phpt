--TEST--
openswoole_socket_coro: accept
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();

co::run(function() use ($port) {
go(function () use ($port) {
    $sock = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    Assert::assert($sock->bind('127.0.0.1', $port));
    Assert::assert($sock->listen(512));
    $conn = $sock->accept();
    Assert::assert($conn);
    Assert::isInstanceOf($conn, OpenSwoole\Coroutine\Socket::class);

    $data = $conn->recv();
    $json = json_decode($data, true);
    Assert::same($json['data'] ?? '', 'hello');
    $conn->send("world\n");
    $conn->close();
});

go(function () use ($port) {
    $conn = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    Assert::assert($conn->connect('127.0.0.1', $port));
    $conn->send(json_encode(['data' => 'hello']));
    echo $conn->recv();
});
});

?>
--EXPECT--
world
