--TEST--
swoole_http_server: llhttp - header parsing (custom headers, case-insensitive, multi-value)
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Socket;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));

        // Test 1: standard headers and custom headers
        $request =
            "GET /headers HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "X-Custom-Header: custom_value\r\n" .
            "X-Request-ID: req-12345\r\n" .
            "Accept: application/json\r\n" .
            "Accept-Language: en-US,en;q=0.9\r\n" .
            "User-Agent: TestClient/1.0\r\n" .
            "Content-Length: 0\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['x-custom-header'], 'custom_value');
        Assert::same($result['x-request-id'], 'req-12345');
        Assert::same($result['accept'], 'application/json');
        Assert::same($result['accept-language'], 'en-US,en;q=0.9');
        Assert::same($result['user-agent'], 'TestClient/1.0');

        // Test 2: header with empty value
        $request =
            "GET /headers HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "X-Empty-Header: \r\n" .
            "X-Normal: value\r\n" .
            "Content-Length: 0\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['x-empty-header'], '');
        Assert::same($result['x-normal'], 'value');

        // Test 3: headers are lowercased
        $request =
            "GET /headers HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "X-UPPER-CASE: upper\r\n" .
            "x-lower-case: lower\r\n" .
            "X-MiXeD-CaSe: mixed\r\n" .
            "Content-Length: 0\r\n" .
            "Connection: close\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['x-upper-case'], 'upper');
        Assert::same($result['x-lower-case'], 'lower');
        Assert::same($result['x-mixed-case'], 'mixed');

        $socket->close();
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
        $response->end(json_encode($request->header));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
