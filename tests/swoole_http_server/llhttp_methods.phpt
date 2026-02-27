--TEST--
swoole_http_server: llhttp - various HTTP methods parsing
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine;
use Swoole\Coroutine\Socket;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $methods = ['GET', 'POST', 'PUT', 'DELETE', 'PATCH', 'OPTIONS', 'HEAD', 'PURGE'];
        $socket = new Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($socket->connect('127.0.0.1', $pm->getFreePort()));

        foreach ($methods as $method) {
            $body = ($method !== 'GET' && $method !== 'HEAD' && $method !== 'OPTIONS' && $method !== 'TRACE')
                ? "test_body_{$method}" : '';
            $headers = "{$method} /test_method HTTP/1.1\r\n" .
                "Host: localhost\r\n" .
                "Connection: Keep-Alive\r\n";
            if (strlen($body) > 0) {
                $headers .= "Content-Length: " . strlen($body) . "\r\n";
            }
            $headers .= "\r\n" . $body;

            $ret = $socket->sendAll($headers);
            Assert::greaterThan($ret, 0);
            $response = $socket->recv(8192, 1);
            Assert::contains($response, "METHOD:{$method}");
        }
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
        $method = $request->server['request_method'];
        $response->end("METHOD:{$method}");
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
