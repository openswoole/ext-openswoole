--TEST--
swoole_curl: sleep
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Http\Request;
use Swoole\Http\Response;
use Swoole\Runtime;
use Swoole\Coroutine;
use Swoole\Coroutine\System;


$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function () use ($pm) {
    Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
    $s = microtime(true);
    co::run(function () use ($pm) {
        $ch = curl_init();
        $code = uniqid('swoole_');
        $url = "http://127.0.0.1:".$pm->getFreePort()."/?code=".urlencode($code);

        curl_setopt($ch, CURLOPT_URL, $url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_HEADER, 0);

        $cid = Coroutine::getCid();
        go(function () use ($cid) {
            System::usleep(10000);
            Assert::true(Coroutine::cancel($cid));
            Assert::false(Coroutine::isCanceled());
        });

        $output = curl_exec($ch);
        Assert::isEmpty($output);
        Assert::eq(\OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANCELED);
        Assert::eq(curl_errno($ch), CURLE_ABORTED_BY_CALLBACK);
        Assert::contains(curl_error($ch), 'Operation was aborted by an application callback');
        curl_close($ch);
    });
    Assert::lessThan(microtime(true) - $s, 0.5);
    $pm->kill();
    echo "Done\n";
};
$pm->childFunc = function () use ($pm) {
    $http = new Swoole\Http\Server("127.0.0.1", $pm->getFreePort());
    $http->set(['worker_num' => 1, 'log_file' => '/dev/null']);

    $http->on("start", function ($server) use ($pm) {
        $pm->wakeup();
    });

    $http->on("request", function (Request $request, Response $response) {
        usleep(300000);
        $response->end("Hello World\n".$request->get['code']);
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
Done
