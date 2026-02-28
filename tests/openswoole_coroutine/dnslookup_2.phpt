--TEST--
openswoole_coroutine: async dns lookup timeout
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;


Co::run(function () {
    $host = OpenSwoole\Coroutine::dnsLookup('www.' . uniqid() . '.' . uniqid(), 0.5);
    Assert::eq($host, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED);
});
?>
--EXPECT--
