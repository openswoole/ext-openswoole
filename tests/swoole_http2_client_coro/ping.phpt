--TEST--
swoole_http2_client_coro: http2 ping
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Co::set(['log_level' => SWOOLE_LOG_WARNING]);
go(function () {
    $cli = new Swoole\Coroutine\Http2\Client('nghttp2.org', 443, true);
    $cli->set(['timeout' => 5,]);
    if (!$cli->connect()) {
        return;
    }
    $ret = $cli->ping();
    Assert::assert($ret);
    co::usleep(500000);
    Assert::greaterThan($cli->send(new Swoole\Http2\Request), 0);
    /** @var $response Swoole\Http2\Response */
    $response = $cli->recv();
    if (!$response && $cli->errCode === SOCKET_ETIMEDOUT) {
        return;
    }
    if ($response && ((string) $response->statusCode)[0] === '5') {
        return;
    }
    Assert::contains($response->data, 'nghttp2');
});
?>
--EXPECT--
