--TEST--
openswoole_socket_coro: shutdown
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();
for ($n = 2; $n--;) {
    $randoms[] = get_safe_random();
}
co::run(function() use ($port, $randoms) {
go(function () use ($port, $randoms) {
    $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    Assert::true($server->bind('127.0.0.1', $port));
    Assert::true($server->listen(512));
    $conn = $server->accept();
    Assert::isInstanceOf($conn, OpenSwoole\Coroutine\Socket::class);
    Assert::same($conn->recv(), array_shift($randoms));
    Assert::greaterThan($conn->send((string)array_shift($randoms)), 0);
    $conn->close();
    $server->close();
});
go(function () use ($port, $randoms) {
    $socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    Assert::true($socket->connect('127.0.0.1', $port));
    Assert::greaterThan($socket->send((string)array_shift($randoms)), 0);
    Assert::same($socket->recv(), array_shift($randoms));
    Assert::true($socket->shutdown(STREAM_SHUT_WR));
    for ($n = MAX_REQUESTS; $n--;) {
        Assert::false($socket->send((string)array_shift($randoms)));
        Assert::same($socket->errCode, SOCKET_EPIPE);
    }
    Assert::assert($socket->shutdown(STREAM_SHUT_RD));
    for ($n = MAX_REQUESTS; $n--;) {
        Assert::assert(!$socket->recv());
    }
    Assert::false($socket->shutdown());
    Assert::same($socket->errCode, SOCKET_ENOTCONN);
    Assert::true($socket->close());
    Assert::false($socket->send(''));
    Assert::false($socket->recv());
    Assert::same($socket->errCode, SOCKET_EBADF);
});
});

echo "DONE\n";
?>
--EXPECTF--
DONE
