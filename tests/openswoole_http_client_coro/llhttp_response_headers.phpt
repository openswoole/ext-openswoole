--TEST--
openswoole_http_client_coro: llhttp - response header parsing
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

        // Test 1: standard response headers
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        Assert::true($cli->get('/headers'));
        Assert::same($cli->headers['x-custom-header'], 'custom_value');
        Assert::same($cli->headers['x-request-id'], 'req-99');
        Assert::same($cli->headers['content-type'], 'application/json');
        Assert::same($cli->headers['x-powered-by'], 'OpenSwoole');

        // Test 2: set-cookie headers
        Assert::true($cli->get('/cookies'));
        Assert::notEmpty($cli->set_cookie_headers);
        $hasSid = false;
        $hasTheme = false;
        foreach ($cli->set_cookie_headers as $cookie) {
            if (str_starts_with($cookie, 'sid=')) {
                $hasSid = true;
            }
            if (str_starts_with($cookie, 'theme=')) {
                $hasTheme = true;
            }
        }
        Assert::true($hasSid);
        Assert::true($hasTheme);

        // Test 3: response content-length
        Assert::true($cli->get('/fixed-body'));
        Assert::same($cli->headers['content-length'], '11');
        Assert::same($cli->body, 'hello world');

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
        if ($uri === '/headers') {
            $response->header('X-Custom-Header', 'custom_value');
            $response->header('X-Request-ID', 'req-99');
            $response->header('Content-Type', 'application/json');
            $response->header('X-Powered-By', 'OpenSwoole');
            $response->end('{}');
        } elseif ($uri === '/cookies') {
            $response->cookie('sid', 'abc123', time() + 3600);
            $response->cookie('theme', 'dark', time() + 3600);
            $response->end('ok');
        } elseif ($uri === '/fixed-body') {
            $response->end('hello world');
        } else {
            $response->end('not found');
        }
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
