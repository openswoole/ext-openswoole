--TEST--
openswoole_client_coro: sendto
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Constant;
use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Client;
use OpenSwoole\Coroutine\Socket;

Coroutine::run(function () {
    $port = get_one_free_port();
    $socket = new Socket(AF_INET, SOCK_DGRAM, 0);
    Assert::true($socket->bind('127.0.0.1', $port));

    go(function () use ($socket) {
        foreach (['packet-1', 'packet-2', 'packet-3'] as $expected) {
            $peer = null;
            Assert::same($socket->recvfrom($peer, 5), $expected);
            Assert::same($peer['address'], '127.0.0.1');
        }
        echo "DONE\n";
    });

    $cli = new Client(Constant::SOCK_UDP);
    Assert::true($cli->sendto('127.0.0.1', $port, 'packet-1'));
    Assert::true($cli->sendto('localhost', $port, 'packet-2'));
    Assert::false($cli->sendto('127.0.0.1', $port, ''));
    Assert::false($cli->sendto('error_domain', $port, 'hello'));
    Assert::same($cli->errCode, Constant::ERROR_DNSLOOKUP_RESOLVE_FAILED);
    Assert::same($cli->errMsg, 'DNS Lookup resolve failed');
    // the client is still usable after a failed lookup
    Assert::true($cli->sendto('127.0.0.1', $port, 'packet-3'));
});
?>
--EXPECT--
DONE
