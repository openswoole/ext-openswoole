--TEST--
swoole_http_client_coro: http client with http_proxy
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_http_proxy();
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    $domain = 'openswoole.com';
    $cli = new OpenSwoole\Coroutine\Http\Client($domain, 443, true);
    // $cli->setHeaders(['Host' => $domain]); // without host header it can also work well
    $cli->set([
        'timeout' => 30,
        'http_proxy_host' => HTTP_PROXY_HOST,
        'http_proxy_port' => HTTP_PROXY_PORT
    ]);
    $result = $cli->get('/');
    Assert::assert($result);
    Assert::assert(stripos($cli->body, 'openswoole') !== false);
    echo "DONE\n";
});
?>
--EXPECT--
DONE
