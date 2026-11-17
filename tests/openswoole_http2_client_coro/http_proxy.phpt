--TEST--
openswoole_http2_client_coro: get with http_proxy
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_http_proxy();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Http2\Client;
use OpenSwoole\Http2\Request;

// the proxy tunnels to an HTTP/2 server it can reach:
// the h2demo container in CI, the local test server otherwise
if (IS_IN_TRAVIS) {
    [$host, $port, $keyword] = ['h2demo', 4430, 'Go + HTTP/2'];
} else {
    $server = SwooleTest\LocalServer::start();
    [$host, $port, $keyword] = [$server->host, $server->httpsPort, 'OpenSwoole test page'];
}

co::run(function () use ($host, $port, $keyword) {
    $c = new Client($host, $port, true);
    $c->set([
        'timeout' => 10,
        'ssl_host_name' => $host,
        'http_proxy_host' => HTTP_PROXY_HOST,
        'http_proxy_port' => HTTP_PROXY_PORT,
    ]);
    Assert::true($c->connect(), var_dump_return($c));
    $r = new Request();
    $r->method = 'GET';
    $r->path = '/';
    $r->headers = [
        'host' => $host,
    ];
    $c->send($r);
    $response = $c->recv();
    Assert::notEmpty($response);
    Assert::eq($response->statusCode, 200);
    Assert::contains($response->data, $keyword);
});
?>
--EXPECT--
