--TEST--
swoole_http_server: llhttp - HTTP pipelining (multiple requests in single connection)
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine;
use Swoole\Coroutine\Socket;

const EOF = "\r\nEOF";

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));
        Assert::true($socket->setProtocol([
            'open_eof_check' => true,
            'package_eof' => EOF,
        ]));

        // Send 3 pipelined requests at once
        $requests = '';
        for ($i = 1; $i <= 3; $i++) {
            $requests .=
                "GET /pipeline?seq={$i} HTTP/1.1\r\n" .
                "Host: localhost\r\n" .
                "Connection: Keep-Alive\r\n" .
                "\r\n";
        }
        $socket->sendAll($requests);

        // Receive 3 responses
        for ($i = 1; $i <= 3; $i++) {
            $response = $socket->recvPacket();
            Assert::contains($response, "seq={$i}");
        }

        // Send requests with different methods (body-bearing requests sent individually
        // since the server protocol layer does not support pipelining after requests with body)
        $body1 = 'data=hello';

        $request = "GET /pipeline?type=get HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recvPacket();
        Assert::contains($response, "type=get");
        Assert::contains($response, "METHOD:GET");

        $request = "POST /pipeline?type=post HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Content-Type: application/x-www-form-urlencoded\r\n" .
            "Content-Length: " . strlen($body1) . "\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n" .
            $body1;
        $socket->sendAll($request);
        $response = $socket->recvPacket();
        Assert::contains($response, "type=post");
        Assert::contains($response, "METHOD:POST");

        $request = "DELETE /pipeline?type=delete HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recvPacket();
        Assert::contains($response, "type=delete");
        Assert::contains($response, "METHOD:DELETE");

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
    $http->on('request', function (Swoole\Http\Request $request, Swoole\Http\Response $response) {
        $qs = $request->server['query_string'] ?? '';
        $method = $request->server['request_method'];
        $response->end("METHOD:{$method}|{$qs}" . EOF);
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
