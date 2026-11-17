--TEST--
openswoole_curl/guzzle: cookie
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
require_once TESTS_LIB_PATH . '/vendor/autoload.php';

use OpenSwoole\Runtime;
use GuzzleHttp\Client;
use GuzzleHttp\Cookie\CookieJar;



$server = SwooleTest\LocalServer::start();
Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () use ($server) {
    $client = new Client();
    $jar = CookieJar::fromArray(
        [
            'some_cookie' => 'foo',
            'other_cookie' => 'barbaz1234'
        ],
        $server->host
    );
    $r = $client->request('GET', $server->url('/cookies'), [
        'cookies' => $jar
    ]);
    Assert::eq($r->getStatusCode(), 200);
    Assert::eq(json_decode($r->getBody()->getContents(), true)['cookies']['some_cookie'], 'foo');
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
Done
