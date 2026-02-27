--TEST--
swoole_http_server: llhttp - query string and path parsing
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

        // Test 1: simple query string
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        Assert::true($cli->get('/path?foo=bar&baz=123'));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/path');
        Assert::same($result['get']['foo'], 'bar');
        Assert::same($result['get']['baz'], '123');
        Assert::same($result['query_string'], 'foo=bar&baz=123');

        // Test 2: URL-encoded query string
        Assert::true($cli->get('/api/test?name=' . urlencode('hello world') . '&val=' . urlencode('a&b=c')));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/api/test');
        Assert::same($result['get']['name'], 'hello world');
        Assert::same($result['get']['val'], 'a&b=c');

        // Test 3: empty query string value
        Assert::true($cli->get('/empty?key=&other=val'));
        $result = json_decode($cli->body, true);
        Assert::same($result['get']['key'], '');
        Assert::same($result['get']['other'], 'val');

        // Test 4: no query string
        Assert::true($cli->get('/no-query'));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/no-query');
        Assert::same($result['query_string'], null);

        // Test 5: root path
        Assert::true($cli->get('/'));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/');

        // Test 6: URL with fragment (should be stripped by client but test server handling)
        Assert::true($cli->get('/page?q=test'));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/page');
        Assert::same($result['get']['q'], 'test');

        // Test 7: nested path
        Assert::true($cli->get('/a/b/c/d?x=1'));
        $result = json_decode($cli->body, true);
        Assert::same($result['path'], '/a/b/c/d');
        Assert::same($result['get']['x'], '1');

        // Test 8: URL-encoded path
        Assert::true($cli->get('/path%20with%20spaces?key=val'));
        $result = json_decode($cli->body, true);
        Assert::same($result['request_uri'], '/path%20with%20spaces');
        Assert::same($result['path_info'], '/path with spaces');
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
        $result = [
            'path' => $request->server['path_info'],
            'request_uri' => $request->server['request_uri'],
            'path_info' => $request->server['path_info'],
            'query_string' => $request->server['query_string'] ?? null,
            'get' => $request->get ?? [],
        ];
        $response->end(json_encode($result));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
