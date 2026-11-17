--TEST--
openswoole_coroutine_system: gethostbyname timeout
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// a local UDP socket that never answers stands in for an unresponsive DNS server
$silentDns = stream_socket_server('udp://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND);
OpenSwoole\Coroutine::set(['dns_server' => stream_socket_get_name($silentDns, false)]);

OpenSwoole\Coroutine::create(function () {
    $result = OpenSwoole\Coroutine\System::gethostbyname('openswoole-' . time() . '.invalid', AF_INET, 0.001);
    Assert::eq($result, false);
    Assert::same(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT);
    co::usleep(100000);
    echo "NEXT\n";
    $result = OpenSwoole\Coroutine\System::gethostbyname('localhost', AF_INET, 1);
    Assert::same($result, '127.0.0.1');
});
openswoole_event_wait();
?>
--EXPECT--
NEXT
