--TEST--
openswoole_http_client_coro: http client with HEAD method
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();
OpenSwoole\Coroutine::create(function () use ($server)
{
    $cli = new \OpenSwoole\Coroutine\Http\Client($server->host, $server->httpPort);
    $cli->set(['timeout' => 10]);
    $cli->setMethod('HEAD');
    $cli->get('/');
    Assert::same($cli->statusCode, 200);
    Assert::assert(count($cli->headers) > 0);
});
?>
--EXPECT--
