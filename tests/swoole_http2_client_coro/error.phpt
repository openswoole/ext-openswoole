--TEST--
swoole_http2_client_coro: cookies
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $cli = new OpenSwoole\Coroutine\Http2\Client('192.0.0.1', 9000);
    $cli->set(['timeout' => 0.1]);
    Assert::false($cli->connect());
    Assert::same($cli->errCode, SOCKET_ETIMEDOUT);
    Assert::false($cli->send(new OpenSwoole\Http2\Request));
    Assert::same($cli->errCode, SWOOLE_ERROR_CLIENT_NO_CONNECTION);
    Assert::false($cli->recv(1));
    Assert::same($cli->errCode, SWOOLE_ERROR_CLIENT_NO_CONNECTION);
});
Swoole\Event::wait();
?>
--EXPECT--
