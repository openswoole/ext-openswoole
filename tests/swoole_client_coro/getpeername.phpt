--TEST--
swoole_client_coro: getpeername
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine\System;

Co::run(
    function () {
        $conn = new Swoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        $conn->connect('www.baidu.com', 80);
        $info = $conn->getpeername();
        Assert::eq($info['host'], System::gethostbyname('www.baidu.com'));
        Assert::eq($info['port'], 80);
    }
);
?>
--EXPECT--
