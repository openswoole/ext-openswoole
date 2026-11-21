--TEST--
openswoole_curl: a request after CURLOPT_PREREQFUNCTION was set back to null does not crash
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
if (!defined('CURLOPT_PREREQFUNCTION')) {
    skip('CURLOPT_PREREQFUNCTION not available (requires libcurl >= 7.80.0)');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Request;
use OpenSwoole\Http\Response;
use OpenSwoole\Runtime;

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function () use ($pm) {
    Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
    co::run(function () use ($pm) {
        $ch = curl_init("http://127.0.0.1:" . $pm->getFreePort() . "/");
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

        // Guzzle sets the callback for a request, and its handle pool sets it back to null when it takes the
        // handle back; libcurl keeps calling the C callback for the handle's next request either way.
        $called = 0;
        curl_setopt($ch, CURLOPT_PREREQFUNCTION, function ($ch, $primaryIp, $localIp, $primaryPort, $localPort) use (&$called) {
            $called++;
            return CURL_PREREQFUNC_OK;
        });
        Assert::eq(curl_exec($ch), "Hello World");
        echo "with the callback: called $called\n";

        curl_setopt($ch, CURLOPT_PREREQFUNCTION, null);
        Assert::eq(curl_exec($ch), "Hello World");
        echo "with it cleared: called $called\n";

        curl_close($ch);
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
with the callback: called 1
with it cleared: called 1
Done
