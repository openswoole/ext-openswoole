--TEST--
swoole_coroutine: bad dns server
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine\System;


Co::run(function () {
    Co::set(['dns_server' => '192.0.0.1:10053']);
    $host = Swoole\Coroutine::dnsLookup('www.baidu.com', 0.5);
    Assert::eq($host, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED);
});
?>
--EXPECT--
