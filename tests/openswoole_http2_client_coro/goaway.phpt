--TEST--
openswoole_http2_client_coro: http2 go away
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $domain = 'nghttp2.org';
    $cli = new OpenSwoole\Coroutine\Http2\Client($domain, 443, true);
    $cli->set([
        'timeout' => 1,
        'ssl_host_name' => $domain
    ]);
    if (!$cli->connect()) {
        return;
    }
    $cli->goaway(SWOOLE_HTTP2_ERROR_NO_ERROR, '[GOAWAY] nothing~bye~bye~');
    Assert::assert(!$cli->recv(-1));
});
?>
--EXPECT--
