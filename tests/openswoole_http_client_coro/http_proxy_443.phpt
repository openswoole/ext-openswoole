--TEST--
openswoole_http_client_coro: http client with http_proxy
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_http_proxy();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
// the proxy tunnels (CONNECT) to an HTTPS server it can reach:
// the h2demo container in CI, the local test server otherwise
if (IS_IN_TRAVIS) {
    [$host, $port, $keyword] = ['h2demo', 4430, 'Go + HTTP/2'];
} else {
    $server = SwooleTest\LocalServer::start();
    [$host, $port, $keyword] = [$server->host, $server->httpsPort, 'OpenSwoole test page'];
}
$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm, $host, $port, $keyword) {
    go(function () use ($pm, $host, $port, $keyword) {
        $cli = new OpenSwoole\Coroutine\Http\Client($host, $port, true);
        $cli->setHeaders(['Host' => $host]);
        $cli->set([
            'timeout'         => 30,
            'http_proxy_host' => HTTP_PROXY_HOST,
            'http_proxy_port' => HTTP_PROXY_PORT
        ]);
        $result = $cli->get('/');
        Assert::assert($result);
        Assert::assert(stripos($cli->body, $keyword) !== false);
        $pm->kill();
    });
};
$pm->childFunc = function () use ($pm) {
    $pm->wakeup();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
