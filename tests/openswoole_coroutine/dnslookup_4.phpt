--TEST--
openswoole_coroutine: bad dns server
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;


// a local UDP socket that never answers stands in for an unresponsive DNS server
$silentDns = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
Co::run(function () use ($silentDns) {
    Co::set(['dns_server' => stream_socket_get_name($silentDns, false)]);
    $host = OpenSwoole\Coroutine::dnsLookup('openswoole.invalid', 0.5);
    Assert::eq($host, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED);
});
?>
--EXPECT--
