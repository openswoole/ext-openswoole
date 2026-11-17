--TEST--
openswoole_http_client_coro: alias
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
    ]);
    $random = get_safe_random(16);
    Assert::assert($cli->get("/cookies/set?a={$random}&b=2&c=3"));
    Assert::contains($cli->getBody(), $random);
    Assert::same($cli->getStatusCode(), 200);
    Assert::assert(count($cli->getHeaders()) > 3);
    Assert::assert(count($cli->getCookies()) > 2);
    Assert::same($cli->getCookies()['a'], $random);
    echo "DONE\n";
});
openswoole_event_wait();
?>
--EXPECT--
DONE
