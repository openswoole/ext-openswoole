--TEST--
swoole_http_client_coro: llhttp - client POST/PUT/PATCH/DELETE with body parsing
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

        // Test 1: POST form data
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        $cli->setMethod('POST');
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setData('field1=value1&field2=value2');
        Assert::true($cli->execute('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'POST');
        Assert::same($result['body'], 'field1=value1&field2=value2');
        Assert::greaterThan((int) $result['content_length'], 0);

        // Test 2: PUT with JSON
        $jsonData = json_encode(['action' => 'update', 'id' => 42]);
        $cli->setMethod('PUT');
        $cli->setHeaders(['Content-Type' => 'application/json']);
        $cli->setData($jsonData);
        Assert::true($cli->execute('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'PUT');
        Assert::same($result['body'], $jsonData);

        // Test 3: PATCH with body
        $cli->setMethod('PATCH');
        $cli->setHeaders(['Content-Type' => 'application/json']);
        $cli->setData('{"name":"patched"}');
        Assert::true($cli->execute('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'PATCH');
        Assert::same($result['body'], '{"name":"patched"}');

        // Test 4: DELETE with body
        $cli->setMethod('DELETE');
        $cli->setHeaders(['Content-Type' => 'application/json']);
        $cli->setData('{"id":99}');
        Assert::true($cli->execute('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'DELETE');
        Assert::same($result['body'], '{"id":99}');

        // Test 5: POST with large body
        $largeData = str_repeat('X', 65536);
        $cli->setMethod('POST');
        $cli->setHeaders(['Content-Type' => 'application/octet-stream']);
        $cli->setData($largeData);
        Assert::true($cli->execute('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'POST');
        Assert::same((int) $result['body_len'], 65536);

        // Test 6: GET request should have no body
        $cli->setData('');
        Assert::true($cli->get('/echo'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'GET');
        Assert::same((int) $result['body_len'], 0);

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
        $body = $request->rawContent();
        $result = [
            'method' => $request->server['request_method'],
            'body' => $body,
            'body_len' => strlen($body),
            'content_length' => $request->header['content-length'] ?? '0',
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
