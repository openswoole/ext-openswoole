--TEST--
openswoole_http_client_coro: reuse defer client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    function createDeferCli(SwooleTest\LocalServer $server, bool $ssl = false): OpenSwoole\Coroutine\Http\Client
    {
        $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $ssl ? $server->httpsPort : $server->httpPort, $ssl);
        $cli->set(['timeout' => 10]);
        $cli->setHeaders([
            'Host' => $server->host,
            'User-Agent' => 'Chrome/49.0.2587.3',
            'Accept' => 'text/html,application/xhtml+xml,application/xml',
            'Accept-Encoding' => 'gzip',
        ]);
        $cli->setDefer(true);

        return $cli;
    }

    $https = createDeferCli($server, true);
    $http = createDeferCli($server);

    //first
    $https->get('/');
    $http->get('/');
    $https->recv(10);
    $http->recv(10);
    Assert::same($https->statusCode, 200);
    Assert::assert(stripos($https->body, 'OpenSwoole') !== false);
    Assert::same($http->statusCode, 200);
    Assert::assert(stripos($http->body, 'OpenSwoole') !== false);

    //reuse
    $https->get('/get?page=docs');
    $http->get('/help');
    $https->recv(10);
    $http->recv(10);
    Assert::same($https->statusCode, 200);
    Assert::assert(stripos($https->body, 'docs') !== false);
    Assert::same($http->statusCode, 404);
    Assert::assert(stripos($http->body, 'Not Found') !== false);
});
?>
--EXPECT--
