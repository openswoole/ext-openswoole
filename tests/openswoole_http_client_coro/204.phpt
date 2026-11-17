--TEST--
openswoole_http_client_coro: http 204 no content
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $domain = $server->host;
    $cli = new OpenSwoole\Coroutine\Http\Client($domain, $server->httpPort);
    $cli->set(['timeout' => 5]);
    $cli->setHeaders([
        'Host' => $domain,
        'User-Agent' => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip'
    ]);
    Assert::assert($cli->post('/status/204', []));
    Assert::same($cli->statusCode, 204);
    Assert::assert(empty($cli->body));
});

?>
--EXPECT--
