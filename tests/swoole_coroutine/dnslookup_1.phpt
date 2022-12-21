--TEST--
swoole_coroutine: \OpenSwoole\Coroutine::dnsLookup
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine\System;


Co::run(function () {
    $host = System::dnsLookup('www.baidu.com');
    Assert::assert(filter_var($host, FILTER_VALIDATE_IP) !== false);
});
?>
--EXPECT--
