--TEST--
openswoole_coroutine: \OpenSwoole\Coroutine::dnsLookup
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;


Co::run(function () {
    $host = System::dnsLookup('localhost');
    Assert::same($host, '127.0.0.1');
});
?>
--EXPECT--
