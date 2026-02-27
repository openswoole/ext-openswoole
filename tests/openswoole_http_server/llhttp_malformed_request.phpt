--TEST--
swoole_http_server: llhttp - malformed HTTP request handling
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
        // Test 1: completely invalid HTTP data
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $socket->sendAll("NOT HTTP DATA AT ALL\r\n\r\n");
        $response = $socket->recv(8192, 2);
        // Server should close connection or return error
        Assert::assert($response === '' || $response === false || str_contains($response, '400'));
        $socket->close();

        // Test 2: incomplete HTTP request line
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $socket->sendAll("GET\r\n\r\n");
        $response = $socket->recv(8192, 2);
        Assert::assert($response === '' || $response === false || str_contains($response, '400'));
        $socket->close();

        // Test 3: valid request after invalid should work on new connection
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        $request =
            "GET /ok HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Connection: close\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, "HTTP/1.1 200");
        Assert::contains($response, "OK");
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
        $response->end("OK");
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
