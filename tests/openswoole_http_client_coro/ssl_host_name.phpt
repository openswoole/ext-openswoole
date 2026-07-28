--TEST--
openswoole_http_client_coro: https client with ssl_host_name
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    $c = new OpenSwoole\Coroutine\http\Client('httpbun.com', 443, true);
    $c->set([
        'timeout' => 5,
        'ssl_host_name' => 'httpbun.com'
    ]);
    $c->get('/');
    Assert::assert(strlen($c->body) > 0);
    Assert::same($c->statusCode, 200);
});
openswoole_event::wait();
?>
--EXPECT--
