--TEST--
openswoole_curl: guzzle
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip("TODOv22");
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
require_once TESTS_LIB_PATH . '/vendor/autoload.php';

use OpenSwoole\Coroutine\WaitGroup;
use OpenSwoole\Runtime;
use GuzzleHttp\Client;
use GuzzleHttp\Promise;

Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

const N = 4;

$server = SwooleTest\LocalServer::start();

co::run(function () use ($server) {
    $wg = new WaitGroup();
    $result = [];
    go(function () use ($wg, &$result, $server) {
        $wg->add();
        $client = new Client(['verify' => false]);
        $promises = [
            'a' => $client->getAsync($server->url('/')),
            'b' => $client->getAsync($server->url('/', true)),
            'c' => $client->getAsync($server->url('/?n=3'))
        ];
        $responses = Promise\Utils::unwrap($promises);
        Assert::contains((string)$responses['a']->getBody(), 'OpenSwoole test page');
        Assert::contains((string)$responses['b']->getBody(), 'OpenSwoole test page');
        Assert::contains((string)$responses['c']->getBody(), 'OpenSwoole test page');
        $result['task_1'] = 'OK';
        $wg->done();
    });

    go(function () use ($wg, &$result, $server) {
        $wg->add();
        $client = new Client(['base_uri' => $server->url('/')]);
        $n = N;
        $data = $promises = [];
        while ($n--) {
            $key = 'req_' . $n;
            $data[$key] = uniqid('openswoole_test');
            $promises[$key] = $client->getAsync('/get', ['query' => ['data' => $data[$key]]]);
        }
        $responses = Promise\Utils::unwrap($promises);

        $n = N;
        while ($n--) {
            $key = 'req_' . $n;
            Assert::eq(json_decode((string)$responses[$key]->getBody(), true)['args']['data'], $data[$key]);
        }
        $result['task_2'] = 'OK';
        $wg->done();
    });

    $wg->wait();
    Assert::eq($result['task_1'], 'OK');
    Assert::eq($result['task_2'], 'OK');
    echo 'Done' . PHP_EOL;
});
?>
--EXPECT--
Done
