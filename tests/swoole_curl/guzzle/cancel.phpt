--TEST--
swoole_curl/guzzle: cancel
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
require_once TESTS_LIB_PATH . '/vendor/autoload.php';

use Swoole\Runtime;
use GuzzleHttp\Handler\CurlMultiHandler;
use GuzzleHttp\Promise;
use GuzzleHttp\Psr7\Request;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () {
    $handler = new CurlMultiHandler();
    $responses = [];
    for ($i = 0; $i < 10; $i++) {
        $response = $handler(new Request('GET', 'http://httpbin.org/get'), []);
        $response->cancel();
        $responses[] = $response;
    }
    foreach ($responses as $r) {
        Assert::true(Promise\Is::rejected($r));
    }
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
Done
