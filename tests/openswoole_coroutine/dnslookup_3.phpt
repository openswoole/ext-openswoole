--TEST--
openswoole_coroutine: async dns lookup [3]
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    $host = \OpenSwoole\Coroutine::dnsLookup('www.' . uniqid() . '.' . uniqid(), 15);
    Assert::eq($host, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED);
});
openswoole_event_wait();
?>
--EXPECT--
