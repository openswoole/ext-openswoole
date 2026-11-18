--TEST--
openswoole_http_client_coro: socks5 proxy
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_socks5_proxy();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// the proxy connects to an HTTPS server it can reach:
// the h2demo container in CI, the local test server otherwise
if (IS_IN_TRAVIS) {
    [$host, $port, $keyword] = ['h2demo', 4430, 'Go + HTTP/2'];
} else {
    $server = SwooleTest\LocalServer::start();
    [$host, $port, $keyword] = [$server->host, $server->httpsPort, 'OpenSwoole test page'];
}

go(function () use ($host, $port, $keyword)
{
    $cli = new OpenSwoole\Coroutine\Http\Client($host, $port, true);

    $cli->setHeaders(['Host' => $host]);
    $cli->set([
        'timeout'     => 5,
        'socks5_host' => SOCKS5_PROXY_HOST,
        'socks5_port' => SOCKS5_PROXY_PORT,
    ]);

    $ret = $cli->get('/');
    if (!$ret)
    {
        die("ERROR\n");
    }
    Assert::same($cli->statusCode, 200);
    Assert::assert(stripos($cli->body, $keyword) !== false);
    $cli->close();
});

openswoole_event::wait();
?>
--EXPECT--
