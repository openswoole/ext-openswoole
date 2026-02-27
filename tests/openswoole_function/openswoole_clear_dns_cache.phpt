--TEST--
openswoole_function: test openswoole_clear_dns_cache
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Coroutine::clearDNSCache();
?>
--EXPECT--
