--TEST--
openswoole_http_client_coro: multi http client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $clients = [];
        foreach (['one', 'two'] as $name) {
            $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort(), true);
            $cli->set(['timeout' => 10]);
            $cli->setHeaders([
                'Host' => "{$name}.example.com",
                'Accept-Encoding' => 'gzip',
            ]);
            // send both requests before receiving either
            $cli->setDefer(true);
            Assert::true($cli->get('/'));
            $clients[$name] = $cli;
        }
        foreach ($clients as $name => $cli) {
            Assert::true($cli->recv());
            Assert::same($cli->statusCode, 200);
            Assert::same($cli->body, str_repeat("hello {$name} ", 1000));
            $cli->close();
        }
        echo "OK\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $http->set([
        'log_file' => '/dev/null',
        'ssl_cert_file' => SSL_FILE_DIR . '/server.crt',
        'ssl_key_file' => SSL_FILE_DIR . '/server.key',
        'http_compression' => true,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $name = explode('.', $request->header['host'])[0];
        $response->end(str_repeat("hello {$name} ", 1000));
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
OK
