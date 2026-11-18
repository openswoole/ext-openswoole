--TEST--
openswoole_http_client_coro: really keep alive
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip('too slow');
if (getenv("SKIP_SLOW_TESTS")) {
    die("skip slow test");
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpsPort, true);
    $cli->set(['timeout' => 5]);
    Assert::assert($cli->get('/'));
    Assert::assert(strpos($cli->body, 'OpenSwoole test page') !== false);
    co::sleep(75);
    Assert::assert($cli->get('/'));
    Assert::assert(strpos($cli->body, 'OpenSwoole test page') !== false);
});
openswoole_event_wait();
echo "OK\n";
?>
--EXPECT--
OK
