--TEST--
openswoole_socket_coro: sendto
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();

const N = 5;
//Server
go(function () use ($port) {
    $socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_DGRAM, 0);
    $socket->bind('127.0.0.1', $port);
    for ($i = 0; $i < N; $i++)
    {
        $peer = null;
        $data = $socket->recvfrom($peer);
        echo "[Server] recvfrom[{$peer['address']}:{$peer['port']}] : $data\n";
        $socket->sendto($peer['address'], $peer['port'], "Swoole: $data");
    }
});

//Client
go(function () use ($port) {
    $socket = new  OpenSwoole\Coroutine\Socket(AF_INET, SOCK_DGRAM, 0);
    for ($i = 0; $i < N; $i++)
    {
        $socket->sendto('127.0.0.1', $port, "hello-".$i);
        $peer = null;
        $data = $socket->recvfrom($peer);
        echo "[Client] recvfrom[{$peer['address']}:{$peer['port']}] : $data\n";
    }
});
openswoole_event_wait();
?>
--EXPECTF--
[Server] recvfrom[127.0.0.1:%d] : hello-0
[Client] recvfrom[127.0.0.1:%d] : Swoole: hello-0
[Server] recvfrom[127.0.0.1:%d] : hello-1
[Client] recvfrom[127.0.0.1:%d] : Swoole: hello-1
[Server] recvfrom[127.0.0.1:%d] : hello-2
[Client] recvfrom[127.0.0.1:%d] : Swoole: hello-2
[Server] recvfrom[127.0.0.1:%d] : hello-3
[Client] recvfrom[127.0.0.1:%d] : Swoole: hello-3
[Server] recvfrom[127.0.0.1:%d] : hello-4
[Client] recvfrom[127.0.0.1:%d] : Swoole: hello-4
