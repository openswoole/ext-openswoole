--TEST--
openswoole_http_client_coro: use timeout and timeout before connect
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
// a listener that never accepts, so the TLS handshake never completes
$silent = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$silentPort = (int) explode(':', stream_socket_get_name($silent, false))[1];
go(function () use ($server, $silentPort) {
    $host = $server->host;
    $requestHeaders = [
        'Host' => $host
    ];

    $cli1 = new OpenSwoole\Coroutine\Http\Client($host, $silentPort, true);
    $cli1->setHeaders($requestHeaders);
    $cli1->set(['timeout' => 0.001]);
    $cli1->setDefer(true);
    Assert::false($cli1->get('/'));
    Assert::false($cli1->recv());
    Assert::same($cli1->errCode, SOCKET_ETIMEDOUT);
    Assert::same($cli1->statusCode, SWOOLE_HTTP_CLIENT_ESTATUS_CONNECT_FAILED);

    $cli2 = new OpenSwoole\Coroutine\Http\Client($host, $server->httpsPort, true);
    $cli2->setHeaders($requestHeaders);
    $cli2->setDefer(true);

    Assert::false($cli1->get('/'));
    Assert::true($cli2->get('/'));
    Assert::false($cli1->recv());
    Assert::true($cli2->recv());

    Assert::same($cli1->errCode, SOCKET_ETIMEDOUT);
    Assert::same($cli1->statusCode, SWOOLE_HTTP_CLIENT_ESTATUS_CONNECT_FAILED);

    Assert::same($cli2->errCode, 0);
    Assert::assert($cli2->statusCode === 200 && strpos($cli2->body, 'OpenSwoole test page') !== false);
});
openswoole_event::wait();
?>
--EXPECT--
