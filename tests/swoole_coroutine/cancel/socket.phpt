--TEST--
swoole_coroutine/cancel: socket
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Coroutine;



Co::run(function() {
    $socket = new Swoole\Coroutine\Socket(AF_INET, SOCK_DGRAM, 0);
    $socket->bind('127.0.0.1', 9601);
    // server
    $cid = go(function () use ($socket) {
        while (true) {
            $peer = null;
            $data = $socket->recvfrom($peer);
            Assert::assert(empty($data));
            Assert::assert($socket->errCode == SOCKET_ECANCELED);
            break;
        }
        echo "DONE\n";
    });

    // client
    co::usleep(100000);
    Assert::true(Coroutine::cancel($cid));
});
?>
--EXPECT--
DONE
