--TEST--
swoole_curl/guzzle: request on_stats
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
use GuzzleHttp\TransferStats;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () {
    $client = new Client();
    $host = 'http://httpbin.org/stream/1024';
    $client->request('GET', $host, [
        'on_stats' => function (TransferStats $stats) use ($host) {
            Assert::eq($stats->getEffectiveUri(), $host);

            // You must check if a response was received before using the
            // response object.
            if ($stats->hasResponse()) {
                Assert::eq($stats->getResponse()->getStatusCode(), 200);
            } else {
                // Error data is handler specific. You will need to know what
                // type of error data your handler uses before using this
                // value.
                var_dump($stats->getHandlerErrorData());
            }
        }
    ]);
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
Done
