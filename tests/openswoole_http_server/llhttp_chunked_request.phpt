--TEST--
openswoole_http_server: llhttp - chunked transfer encoding request body
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Socket;

function generateChunkedBody(array $chunks): string {
    $body = '';
    foreach ($chunks as $chunk) {
        $body .= dechex(strlen($chunk)) . "\r\n" . $chunk . "\r\n";
    }
    $body .= "0\r\n\r\n";
    return $body;
}

function recvHttpResponse(Socket $socket, float $timeout = 2): string {
    $data = '';
    $headerEnd = false;
    $contentLength = -1;
    $bodyStart = 0;
    while (true) {
        $chunk = $socket->recv(65536, $timeout);
        if ($chunk === false || $chunk === '') break;
        $data .= $chunk;
        if (!$headerEnd) {
            $pos = strpos($data, "\r\n\r\n");
            if ($pos !== false) {
                $headerEnd = true;
                $bodyStart = $pos + 4;
                if (preg_match('/Content-Length:\s*(\d+)/i', substr($data, 0, $pos), $m)) {
                    $contentLength = (int)$m[1];
                }
            }
        }
        if ($headerEnd) {
            if ($contentLength >= 0 && strlen($data) >= $bodyStart + $contentLength) {
                break;
            }
            if ($contentLength < 0) {
                break;
            }
        }
    }
    return $data;
}

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));

        // Test 1: simple chunked body
        $chunks = ['Hello', ' ', 'World'];
        $chunkedBody = generateChunkedBody($chunks);
        $request =
            "POST /chunked HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Transfer-Encoding: chunked\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n" .
            $chunkedBody;
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, 'Hello World');

        // Test 2: empty chunked body
        $request =
            "POST /chunked HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Transfer-Encoding: chunked\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n" .
            "0\r\n\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        Assert::contains($response, 'BODY_LEN:0');

        // Test 3: large chunked body
        $largeChunk = str_repeat('X', 4096);
        $chunks = [$largeChunk, $largeChunk, 'end'];
        $chunkedBody = generateChunkedBody($chunks);
        $request =
            "POST /chunked HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Transfer-Encoding: chunked\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n" .
            $chunkedBody;
        $socket->sendAll($request);
        $response = recvHttpResponse($socket, 2);
        Assert::contains($response, 'BODY_LEN:' . (4096 + 4096 + 3));

        // Test 4: many small chunks
        $chunks = [];
        for ($i = 0; $i < 50; $i++) {
            $chunks[] = "c{$i}";
        }
        $chunkedBody = generateChunkedBody($chunks);
        $request =
            "POST /chunked HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Transfer-Encoding: chunked\r\n" .
            "Connection: close\r\n" .
            "\r\n" .
            $chunkedBody;
        $socket->sendAll($request);
        $response = recvHttpResponse($socket, 2);
        $expectedBody = implode('', $chunks);
        Assert::contains($response, 'BODY_LEN:' . strlen($expectedBody));

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
        $body = $request->rawContent();
        $response->end("BODY_LEN:" . strlen($body) . "|" . $body);
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
