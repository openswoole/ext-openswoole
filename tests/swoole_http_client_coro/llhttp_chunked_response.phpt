--TEST--
swoole_http_client_coro: llhttp - chunked transfer encoding response parsing
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

        // Test 1: simple chunked response
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        Assert::true($cli->get('/chunked-simple'));
        Assert::same($cli->body, 'HelloWorld');
        Assert::same($cli->statusCode, 200);

        // Test 2: large chunked response
        Assert::true($cli->get('/chunked-large'));
        $expected = str_repeat('A', 8192) . str_repeat('B', 8192) . 'END';
        Assert::same(strlen($cli->body), strlen($expected));
        Assert::same($cli->body, $expected);

        // Test 3: many small chunks
        Assert::true($cli->get('/chunked-many'));
        $expected = '';
        for ($i = 0; $i < 100; $i++) {
            $expected .= "chunk{$i}";
        }
        Assert::same($cli->body, $expected);

        // Test 4: empty chunked response
        Assert::true($cli->get('/chunked-empty'));
        Assert::same($cli->body, '');

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
    $http->on('request', function (Swoole\Http\Request $request, Swoole\Http\Response $response) {
        $uri = $request->server['request_uri'];
        if ($uri === '/chunked-simple') {
            $response->write('Hello');
            $response->write('World');
            $response->end();
        } elseif ($uri === '/chunked-large') {
            $response->write(str_repeat('A', 8192));
            $response->write(str_repeat('B', 8192));
            $response->write('END');
            $response->end();
        } elseif ($uri === '/chunked-many') {
            for ($i = 0; $i < 100; $i++) {
                $response->write("chunk{$i}");
            }
            $response->end();
        } elseif ($uri === '/chunked-empty') {
            $response->end('');
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
