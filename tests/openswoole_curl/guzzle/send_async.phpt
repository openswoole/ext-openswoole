--TEST--
openswoole_curl/guzzle: send async
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
use GuzzleHttp\Psr7\Request;



$server = SwooleTest\LocalServer::start();
Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () use ($server) {
    $client = new Client(['verify' => false]);
    $response = $client->request('GET', $server->url('/get', true));

    echo $response->getStatusCode(), PHP_EOL; // 200
    echo $response->getHeaderLine('content-type'), PHP_EOL; // 'application/json'

    // Send an asynchronous request.
    $request = new Request('GET', $server->url('/', true));
    $promise = $client->sendAsync($request)->then(function ($response) {
        echo 'I completed! ' . $response->getStatusCode() . PHP_EOL;
    });

    $promise->wait();
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
200
application/json
I completed! 200
Done
