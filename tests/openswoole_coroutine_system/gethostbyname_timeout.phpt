--TEST--
openswoole_coroutine_system: gethostbyname timeout
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Coroutine::create(function () {
    $result = OpenSwoole\Coroutine\System::gethostbyname("wwww.xxxx.cccn.xer" . time(), AF_INET, 0.001);
    Assert::eq($result, false);
    Assert::same(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DNSLOOKUP_RESOLVE_TIMEOUT);
    co::usleep(100000);
    echo "NEXT\n";
    $result = OpenSwoole\Coroutine\System::gethostbyname("www.github.com", AF_INET, 1);
    Assert::notEmpty($result);
});
openswoole_event_wait();
?>
--EXPECT--
NEXT
