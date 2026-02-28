--TEST--
openswoole_client_coro: close in other coroutine
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$cid = go(function () {
    $sock = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    Assert::assert($sock->bind('127.0.0.1', 9601));
    Assert::assert($sock->listen(512));
    $conn = $sock->accept();
    Assert::assert($conn);
    Assert::isInstanceOf($conn, OpenSwoole\Coroutine\Socket::class);
    Co::yield();
});

$client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);

go(function () use ($client) {
    $client->connect('127.0.0.1', 9601);
    $data = @$client->recv();
    //socket is closed
    Assert::assert(!$data && $client->errCode === SOCKET_ECONNRESET);
});

go(function () use ($client, $cid) {
    co::usleep(10000);
    $client->close();
    co::usleep(10000);
    co::resume($cid);
});

openswoole_event_wait();
echo "DONE\n";
?>
--EXPECT--
DONE
