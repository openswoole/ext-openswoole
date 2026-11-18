--TEST--
openswoole_http_client_coro: cookies set bug
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function getCookies()
{
    $result = [];
    var_dump($result); // must be empty array
    return $result;
}

$server = SwooleTest\LocalServer::start();
co::run(function () use ($server) {
    $domain = $server->host;
    $cli = new OpenSwoole\Coroutine\Http\Client($domain, $server->httpPort);
    $cli->set(['timeout' => 5]);
    $cli->setHeaders([
        'Host' => $domain,
        'User-Agent' => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip'
    ]);

    // first request
    $cookies = getCookies();
    $cli->setCookies($cookies);

    Assert::assert($cli->get('/cookies/set?a=1'));

    // second request
    $cookies = getCookies();
    $cli->setCookies($cookies);

    Assert::assert($cli->get('/cookies'));
});

?>
--EXPECTF--
array(0) {
}
array(0) {
}
