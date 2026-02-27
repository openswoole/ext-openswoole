--TEST--
swoole_http_client_coro: llhttp - response status code parsing (2xx, 3xx, 4xx, 5xx)
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine;
use Swoole\Coroutine\Http\Client;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $port = $pm->getFreePort();
        $statusCodes = [200, 201, 204, 301, 302, 304, 400, 401, 403, 404, 405, 500, 502, 503];

        foreach ($statusCodes as $code) {
            $cli = new Client('127.0.0.1', $port);
            $cli->set(['timeout' => 5]);
            Assert::true($cli->get("/status?code={$code}"));
            Assert::same($cli->statusCode, $code);
            $cli->close();
        }
    });
    echo "SUCCESS\n";
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'http_compression' => false,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (Swoole\Http\Request $request, Swoole\Http\Response $response) {
        $code = (int) ($request->get['code'] ?? 200);
        $response->status($code);
        if ($code !== 204 && $code !== 304) {
            $response->end("status:{$code}");
        } else {
            $response->end();
        }
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
