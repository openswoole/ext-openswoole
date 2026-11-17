--TEST--
openswoole_http_client_coro: error handler
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpsPort, true);
    $cli->set([
        'timeout' => 10,
        'ssl_host_name' => 'localhost'
    ]);
    $random = get_safe_random(16);
    Assert::assert($cli->get('/get?foo=' . $random));
    Assert::assert(strpos($cli->body, $random) !== false);
    echo "DONE\n";
});
openswoole_event_wait();
?>
--EXPECT--
DONE
