--TEST--
openswoole_coroutine: dnslookup query hosts
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\System;


Co::run(function () {
    Assert::eq(System::dnsLookup('localhost', 3, AF_INET), '127.0.0.1');
});

?>
--EXPECT--
