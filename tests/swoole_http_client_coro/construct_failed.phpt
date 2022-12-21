--TEST--
swoole_http_client_coro: construct failed
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$http = new OpenSwoole\Coroutine\http\Client('');
?>
--EXPECTF--
Fatal error: Uncaught OpenSwoole\Coroutine\Http\Client\Exception: host is empty in %s/tests/swoole_http_client_coro/construct_failed.php:3
Stack trace:
#0 %s/tests/swoole_http_client_coro/construct_failed.php(3): OpenSwoole\Coroutine\Http\Client->__construct('')
#1 {main}
  thrown in %s/tests/swoole_http_client_coro/construct_failed.php on line 3
