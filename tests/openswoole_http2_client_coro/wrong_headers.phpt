--TEST--
openswoole_http2_client_coro: http2 with wrong headers
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
co::run(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Http2\Client($server->host, $server->httpsPort, true);
    $cli->set([
        'timeout' => 10,
    ]);
    $cli->connect();

    $req = new OpenSwoole\Http2\Request;
    $req->path = '/';
    $req->headers = 1;
    Assert::assert($cli->send($req));
    Assert::assert(is_array($req->headers)); // check array
    /**@var $response openswoole_http2_response */
    $response = $cli->recv();
    echo $response->statusCode;
    Assert::assert(stripos($response->data, 'OpenSwoole test page') !== false);
});
?>
--EXPECT--
200
