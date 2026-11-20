--TEST--
openswoole_http_server: response create with coroutine socket
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::run(function () {
    $port = get_one_free_port();
    $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    Assert::true($server->bind('127.0.0.1', $port));
    Assert::true($server->listen());
    go(function () use ($server) {
        for ($i = 0; $i < 2; $i++) {
            $conn = $server->accept();
            Assert::isInstanceOf($conn, OpenSwoole\Coroutine\Socket::class);
            Assert::contains($conn->recv(), "GET / HTTP/1.1\r\n");
            $resp = OpenSwoole\Http\Response::create($conn);
            if ($i === 0) {
                $resp->header('X-Test', 'ok');
                Assert::true($resp->end('hello world'));
            }
            // the second response is destroyed without end()
            unset($resp);
            $conn->close();
        }
        $server->close();
    });

    for ($i = 0; $i < 2; $i++) {
        $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($client->connect('127.0.0.1', $port));
        $client->send("GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n");
        $response = '';
        while (($chunk = $client->recv(1)) !== '' && $chunk !== false) {
            $response .= $chunk;
        }
        if ($i === 0) {
            Assert::startsWith($response, "HTTP/1.1 200 OK\r\n");
            Assert::contains($response, "X-Test: ok\r\n");
            Assert::endsWith($response, "\r\n\r\nhello world");
        } else {
            Assert::startsWith($response, "HTTP/1.1 500 Internal Server Error\r\n");
        }
        $client->close();
    }
    echo "DONE\n";
});
?>
--EXPECT--
DONE
