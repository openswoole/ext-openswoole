--TEST--
swoole_http_client_coro: llhttp - keep-alive connection reuse across multiple requests
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Http\Client;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $port = $pm->getFreePort();

        // Test 1: reuse same client for multiple GET requests
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5, 'keep_alive' => true]);

        for ($i = 1; $i <= 10; $i++) {
            Assert::true($cli->get("/reuse?n={$i}"));
            Assert::same($cli->statusCode, 200);
            Assert::contains($cli->body, "n={$i}");
        }

        // Test 2: mix GET and POST on same connection
        Assert::true($cli->get('/method'));
        Assert::contains($cli->body, 'GET');

        $cli->setMethod('POST');
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setData('key=value');
        Assert::true($cli->execute('/method'));
        Assert::contains($cli->body, 'POST');

        Assert::true($cli->get('/method'));
        Assert::contains($cli->body, 'GET');

        // Test 3: request with different paths on same connection
        Assert::true($cli->get('/path/a'));
        Assert::contains($cli->body, '/path/a');
        Assert::true($cli->get('/path/b'));
        Assert::contains($cli->body, '/path/b');
        Assert::true($cli->get('/path/c'));
        Assert::contains($cli->body, '/path/c');

        $cli->close();

        // Test 4: keep_alive disabled creates new connection per request
        $cli2 = new Client('127.0.0.1', $port);
        $cli2->set(['timeout' => 5, 'keep_alive' => false]);
        Assert::true($cli2->get('/test1'));
        Assert::same($cli2->statusCode, 200);
        Assert::true($cli2->get('/test2'));
        Assert::same($cli2->statusCode, 200);
        $cli2->close();
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
    $http->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $uri = $request->server['request_uri'];
        $qs = $request->server['query_string'] ?? '';
        $method = $request->server['request_method'];
        $response->end("URI:{$uri}|QS:{$qs}|{$method}");
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
