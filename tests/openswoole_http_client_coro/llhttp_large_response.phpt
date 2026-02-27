--TEST--
swoole_http_client_coro: llhttp - large response body and content-length handling
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

        // Test 1: 1KB response
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 10]);
        Assert::true($cli->get('/size?n=1024'));
        Assert::same(strlen($cli->body), 1024);
        Assert::same($cli->statusCode, 200);

        // Test 2: 64KB response
        Assert::true($cli->get('/size?n=65536'));
        Assert::same(strlen($cli->body), 65536);

        // Test 3: 256KB response
        Assert::true($cli->get('/size?n=262144'));
        Assert::same(strlen($cli->body), 262144);

        // Test 4: 1MB response
        Assert::true($cli->get('/size?n=1048576'));
        Assert::same(strlen($cli->body), 1048576);

        // Test 5: zero-length body
        Assert::true($cli->get('/size?n=0'));
        Assert::same(strlen($cli->body), 0);

        // Test 6: response without content-length (chunked)
        Assert::true($cli->get('/no-content-length'));
        Assert::same($cli->body, 'response without explicit content-length');

        $cli->close();
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
        if ($uri === '/size') {
            $n = (int) ($request->get['n'] ?? 0);
            $data = $n > 0 ? str_repeat('A', $n) : '';
            $response->end($data);
        } elseif ($uri === '/no-content-length') {
            $response->write('response without explicit content-length');
            $response->end();
        } else {
            $response->end('unknown');
        }
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
