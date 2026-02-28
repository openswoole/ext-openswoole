--TEST--
openswoole_backward_compat: SWOOLE_SSL_* constant aliases
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::assert(defined('SWOOLE_SSL'));
Assert::assert(defined('SWOOLE_SSL_TLSv1'));
Assert::assert(defined('SWOOLE_SSL_TLSv1_1'));
Assert::assert(defined('SWOOLE_SSL_TLSv1_2'));
Assert::assert(defined('SWOOLE_SSL_TLSv1_3'));
Assert::assert(defined('SWOOLE_SSLv23_METHOD'));
Assert::assert(defined('SWOOLE_TLS_METHOD'));
Assert::assert(defined('SWOOLE_TLS_CLIENT_METHOD'));
Assert::assert(defined('SWOOLE_TLS_SERVER_METHOD'));

Assert::same(SWOOLE_SSL, OPENSWOOLE_SSL);
Assert::same(SWOOLE_SSL_TLSv1, OPENSWOOLE_SSL_TLSv1);
Assert::same(SWOOLE_SSL_TLSv1_1, OPENSWOOLE_SSL_TLSv1_1);
Assert::same(SWOOLE_SSL_TLSv1_2, OPENSWOOLE_SSL_TLSv1_2);
Assert::same(SWOOLE_SSL_TLSv1_3, OPENSWOOLE_SSL_TLSv1_3);
Assert::same(SWOOLE_SSLv23_METHOD, OPENSWOOLE_SSLv23_METHOD);
Assert::same(SWOOLE_TLS_METHOD, OPENSWOOLE_TLS_METHOD);

echo "PASSED\n";
?>
--EXPECT--
PASSED
