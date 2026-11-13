--TEST--
openswoole_curl: CURLOPT_HEADERFUNCTION does not leak the curl handle (#391)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Request;
use OpenSwoole\Http\Response;
use OpenSwoole\Runtime;

$pm = new SwooleTest\ProcessManager;

const N = 20;

$pm->parentFunc = function () use ($pm) {
    Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
    co::run(function () use ($pm) {
        $handles = [];
        $headers = 0;
        for ($i = 0; $i < N; $i++) {
            $ch = curl_init("http://127.0.0.1:" . $pm->getFreePort() . "/");
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_HEADERFUNCTION, function ($ch, $strHeader) use (&$headers) {
                $headers++;
                return strlen($strHeader);
            });
            Assert::eq(curl_exec($ch), "Hello World");
            $handles[] = WeakReference::create($ch);
            unset($ch);
        }
        Assert::greaterThan($headers, N);
        gc_collect_cycles();
        $alive = count(array_filter($handles, fn (WeakReference $ref) => $ref->get() !== null));
        echo "alive handles: $alive\n";
    });
    $pm->kill();
    echo "Done\n";
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server("127.0.0.1", $pm->getFreePort());
    $http->set(['worker_num' => 1, 'log_file' => '/dev/null']);
    $http->on("start", function ($server) use ($pm) {
        $pm->wakeup();
    });
    $http->on("request", function (Request $request, Response $response) {
        $response->end("Hello World");
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
alive handles: 0
Done
