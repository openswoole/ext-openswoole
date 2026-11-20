--TEST--
openswoole_http_client_coro: really keep alive
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Http\Client;

$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    foreach ([[$server->httpPort, false], [$server->httpsPort, true]] as [$port, $ssl]) {
        $cli = new Client($server->host, $port, $ssl);
        $cli->set(['timeout' => 5]);
        Assert::true($cli->get('/'));
        Assert::same($cli->statusCode, 200);
        Assert::contains($cli->body, 'OpenSwoole test page');
        $first = $cli->getsockname();
        Assert::isArray($first);

        // the idle connection is reused for the next requests
        co::sleep(2);
        Assert::true($cli->get('/'));
        Assert::same($cli->statusCode, 200);
        Assert::contains($cli->body, 'OpenSwoole test page');
        Assert::same($cli->getsockname(), $first);
        Assert::true($cli->get('/get'));
        Assert::same($cli->statusCode, 200);
        Assert::same($cli->getsockname(), $first);
        $cli->close();
    }
});
OpenSwoole\Event::wait();
echo "OK\n";
?>
--EXPECT--
OK
