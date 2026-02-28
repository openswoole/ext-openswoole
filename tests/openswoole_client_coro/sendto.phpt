--TEST--
openswoole_client_coro: sendto
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip('TODOv22');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    $port = get_one_free_port();

    go(function () use ($port) {
        $socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_DGRAM, 0);
        $socket->bind('127.0.0.1', $port);
        $peer = null;
        $ret = $socket->recvfrom($peer);
        Assert::assert($ret, 'hello');
        $ret = $socket->recvfrom($peer);
        Assert::assert($ret, 'hello');
        echo "DONE\n";
    });

    go(function () use ($port) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_UDP);
        $cli->sendto('127.0.0.1', $port, "hello\n");
        $cli->sendto('localhost', $port, "hello\n");
        Assert::false($cli->sendto('error_domain', $port, "hello\n"));
        Assert::assert($cli->errCode, 704);
        Assert::assert($cli->errMsg, 'DNS Lookup resolve failed');
    });
});

?>
--EXPECT--
DONE
