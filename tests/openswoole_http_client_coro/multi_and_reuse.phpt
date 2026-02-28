--TEST--
openswoole_http_client_coro: reuse defer client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    function createDeferCli(string $host, bool $ssl = false): OpenSwoole\Coroutine\Http\Client
    {
        $cli = new OpenSwoole\Coroutine\Http\Client($host, $ssl ? 443 : 80, $ssl);
        $cli->set(['timeout' => 10]);
        $cli->setHeaders([
            'Host' => $host,
            'User-Agent' => 'Chrome/49.0.2587.3',
            'Accept' => 'text/html,application/xhtml+xml,application/xml',
            'Accept-Encoding' => 'gzip',
        ]);
        $cli->setDefer(true);

        return $cli;
    }

    $swoole = createDeferCli('openswoole.com', true);
    $google = createDeferCli('www.google.com', true);

    //first
    $swoole->get('/');
    $google->get('/');
    $swoole->recv(10);
    $google->recv(10);
    Assert::same($swoole->statusCode, 200);
    Assert::assert(stripos($swoole->body, 'OpenSwoole') !== false);
    Assert::same($google->statusCode, 200);
    Assert::assert(stripos($google->body, 'google') !== false);

    //reuse
    $swoole->get('/docs');
    $google->get('/help');
    $swoole->recv(10);
    $google->recv(10);
    Assert::same($swoole->statusCode, 200);
    Assert::assert(stripos($swoole->body, 'OpenSwoole') !== false);
    Assert::same($google->statusCode, 404);
    Assert::assert(stripos($google->body, 'google') !== false);
});
?>
--EXPECT--
