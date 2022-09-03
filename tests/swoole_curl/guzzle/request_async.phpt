--TEST--
swoole_curl/guzzle: request async
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
require_once TESTS_LIB_PATH . '/vendor/autoload.php';

use Swoole\Runtime;
use GuzzleHttp\Client;
use Psr\Http\Message\ResponseInterface;
use GuzzleHttp\Exception\RequestException;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () {
    $client = new Client();
    $promise = $client->requestAsync('GET', 'http://httpbin.org/get');
    $promise->then(
        function (ResponseInterface $res) {
            echo $res->getStatusCode() .PHP_EOL;
        },
        function (RequestException $e) {
            echo $e->getMessage() . PHP_EOL;
            echo $e->getRequest()->getMethod() . PHP_EOL;
        }
    );
    $promise->wait();
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
200
Done
