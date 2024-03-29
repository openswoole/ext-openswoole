--TEST--
swoole_http2_client_coro: connect twice
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    $client = new \OpenSwoole\Coroutine\Http2\Client('www.zhihu.com', 443, true);
    $chan   = new \OpenSwoole\Coroutine\Channel(1);
    go(function () use ($client, $chan) {
        $client->connect();
        $req = new \OpenSwoole\Http2\Request();
        $req->method = 'GET';
        $req->path   = '/io?io=' . str_repeat('xxx', 1000);
        $client->send($req);
        $chan->push(true);
        $resp = $client->recv();
        Assert::eq($resp->statusCode, 200);
        Assert::contains($resp->data, '知乎');
        $chan->pop();
    });
    go(function () use ($client, $chan) {
        Assert::eq($client->connect(), false);
        $req = new \OpenSwoole\Http2\Request();
        $req->method = 'GET';
        $req->path   = '/io?io=xxx';
        $client->send($req);
        $chan->push(true);
        Assert::eq($client->recv(), false);
        $chan->pop();
    });
});

?>
--EXPECT--
