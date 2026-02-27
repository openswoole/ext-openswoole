--TEST--
openswoole_runtime/file_lock: lock_sh_1
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
die("skip not support");
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
const FILE = __DIR__ . '/test.data';
\OpenSwoole\Runtime::enableCoroutine();
$startTime = microtime(true);
go(function () {
    $f = fopen(FILE, 'w+');
    $ret = flock($f, LOCK_EX);
    Assert::assert($ret);
    Co::usleep(300000);
    $ret = flock($f, LOCK_UN);
    Assert::assert($ret);
});

go(function () {
    $f = fopen(FILE, 'w+');
    $ret = flock($f, LOCK_SH);
    Assert::assert($ret);
    co::sleep(2);
    $ret = flock($f, LOCK_UN);
    Assert::assert($ret);
});

go(function () use ($startTime) {
    $f = fopen(FILE, 'w+');
    $ret = flock($f, LOCK_SH);
    Assert::assert($ret);
    Assert::assert((microtime(true) - $startTime) < 1);
    $ret = flock($f, LOCK_UN);
    Assert::assert($ret);
});
openswoole_event_wait();
unlink(FILE);
?>
--EXPECTF--
