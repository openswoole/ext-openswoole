--TEST--
swoole_http_server: llhttp - cookie parsing (multiple cookies, URL-encoded, quoted values)
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

        // Test 1: simple cookies
        $request =
            "GET /cookies HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Cookie: name=value; session=abc123\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['name'], 'value');
        Assert::same($result['session'], 'abc123');

        // Test 2: URL-encoded cookie values
        $request =
            "GET /cookies HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Cookie: data=" . urlencode('hello world') . "; path=" . urlencode('/foo/bar') . "\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['data'], 'hello world');
        Assert::same($result['path'], '/foo/bar');

        // Test 3: quoted cookie value
        $request =
            "GET /cookies HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Cookie: token=\"quoted_value\"; id=42\r\n" .
            "Connection: Keep-Alive\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        Assert::same($result['token'], 'quoted_value');
        Assert::same($result['id'], '42');

        // Test 4: many cookies
        $cookie_parts = [];
        for ($i = 0; $i < 20; $i++) {
            $cookie_parts[] = "key{$i}=val{$i}";
        }
        $request =
            "GET /cookies HTTP/1.1\r\n" .
            "Host: localhost\r\n" .
            "Cookie: " . implode('; ', $cookie_parts) . "\r\n" .
            "Connection: close\r\n" .
            "\r\n";
        $socket->sendAll($request);
        $response = $socket->recv(8192, 1);
        $body = explode("\r\n\r\n", $response, 2)[1] ?? '';
        $result = json_decode($body, true);
        for ($i = 0; $i < 20; $i++) {
            Assert::same($result["key{$i}"], "val{$i}");
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
    $http->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $response->end(json_encode($request->cookie ?? []));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
