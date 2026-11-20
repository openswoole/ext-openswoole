--TEST--
openswoole_client_coro: peek with invalid length
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::run(function () {
    $port = get_one_free_port();
    $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    Assert::true($server->bind('127.0.0.1', $port));
    Assert::true($server->listen());
    go(function () use ($server) {
        $conn = $server->accept();
        $conn->send(str_repeat('A', 3000));
        Co::usleep(300000);
        $conn->close();
        $server->close();
    });

    $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    Assert::true($client->connect('127.0.0.1', $port));
    Co::usleep(100000);
    Assert::false($client->peek(-1));
    Assert::same($client->errCode, SOCKET_EINVAL);
    Assert::false($client->peek(0));
    Assert::same($client->errCode, SOCKET_EINVAL);
    Assert::same($client->peek(10), str_repeat('A', 10));
    $client->close();
    echo "DONE\n";
});
?>
--EXPECT--
DONE
