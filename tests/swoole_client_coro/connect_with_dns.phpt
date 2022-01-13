--TEST--
swoole_client_coro: connect with dns
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co\run(function () {
    $cli = new Swoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect('openswoole.com', 80));
});

?>
--EXPECT--
