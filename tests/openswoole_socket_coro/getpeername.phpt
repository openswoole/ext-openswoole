--TEST--
openswoole_socket_coro: getpeername
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;

co::run(
    function () {
        $conn = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        $conn->connect('www.baidu.com', 80);
        $info = $conn->getpeername();
        Assert::eq($info['address'], System::gethostbyname('www.baidu.com'));
        Assert::eq($info['port'], 80);
    }
);
?>
--EXPECT--
