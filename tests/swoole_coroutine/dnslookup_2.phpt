--TEST--
swoole_coroutine: async dns lookup timeout
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine\System;


Co::run(function () {
    $host = Swoole\Coroutine::dnsLookup('www.' . uniqid() . '.' . uniqid(), 0.5);
    Assert::eq($host, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED);
});
?>
--EXPECT--
