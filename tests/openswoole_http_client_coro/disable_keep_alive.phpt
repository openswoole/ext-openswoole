--TEST--
openswoole_http_client_coro: disable keep alive
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $host = $server->host;
    $cli = new OpenSwoole\Coroutine\Http\Client($host, $server->httpsPort, true);
    $cli->set([
        'timeout' => 10,
        'keep_alive' => false
    ]);
    $cli->setHeaders(['Host' => $host]);
    $cli->get('/');
    Assert::same($cli->statusCode, 200);

    Assert::true($cli->get('/get'));
    Assert::same($cli->statusCode, 200);

    // failed clear
    $cli->set([
        'timeout' => 0.001
    ]);
    Assert::false($cli->get('/delay/1'));
    Assert::assert(empty($cli->headers));
    Assert::assert(empty($cli->body));
});
?>
--EXPECT--
