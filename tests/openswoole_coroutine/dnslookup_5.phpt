--TEST--
openswoole_coroutine: async dns lookup [5]
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;


Co::run(function () {
    $ip = System::dnsLookup('localhost');
    Assert::eq($ip, '127.0.0.1');
});
?>
--EXPECT--
