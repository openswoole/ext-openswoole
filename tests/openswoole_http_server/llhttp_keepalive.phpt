--TEST--
openswoole_http_server: llhttp - keep-alive connection reuse and HTTP version handling
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
        // Test 1: HTTP/1.1 keep-alive (default behavior, multiple requests on same connection)
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));

        for ($i = 1; $i <= 5; $i++) {
            $request =
                "GET /keepalive?n={$i} HTTP/1.1\r\n" .
                "Host: localhost\r\n" .
                "Connection: Keep-Alive\r\n" .
                "\r\n";
            $socket->sendAll($request);
            $response = $socket->recv(8192, 1);
            Assert::contains($response, "HTTP/1.1 200");
            Assert::contains($response, "n={$i}");
        }
        $socket->close();

        // Test 2: HTTP/1.1 with Connection: close should close after response
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $request =
            "GET /close HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Connection: close\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, "HTTP/1.1 200");
        Assert::contains($response, "Connection: close");
        $socket->close();

        // Test 3: HTTP/1.0 request
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $request =
            "GET /http10 HTTP/1.0\r\n" .
            "Host: localhost\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, "version=100");
        $socket->close();

        // Test 4: HTTP/1.1 request
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $request =
            "GET /http11 HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Connection: close\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, "version=101");
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
        $uri = $request->server['request_uri'];
        $version = $request->server['server_protocol'];
        if (str_starts_with($uri, '/keepalive')) {
            $response->end("n=" . ($request->get['n'] ?? ''));
        } elseif ($uri === '/close') {
            $response->end("closed");
        } elseif ($uri === '/http10') {
            $v = ($version === 'HTTP/1.0') ? '100' : 'unknown';
            $response->end("version={$v}");
        } elseif ($uri === '/http11') {
            $v = ($version === 'HTTP/1.1') ? '101' : 'unknown';
            $response->end("version={$v}");
        } else {
            $response->end("ok");
        }
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
