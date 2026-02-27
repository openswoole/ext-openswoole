--TEST--
openswoole_runtime/file_hook: file_put_contents with LOCK_NB
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
if (PHP_VERSION_ID >= 80500) die("Skipped: flock LOCK_NB behavior changed in PHP 8.5");
$f = tempnam(sys_get_temp_dir(), 'lk');
$fp1 = fopen($f, 'w+'); flock($fp1, LOCK_EX);
$fp2 = fopen($f, 'w+'); $r = flock($fp2, LOCK_EX | LOCK_NB);
fclose($fp1); fclose($fp2); unlink($f);
if ($r !== false) die("skip: LOCK_NB not supported in this environment");
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

swoole\runtime::enableCoroutine();

const FILE = __DIR__ . '/test.data';

OpenSwoole\Runtime::enableCoroutine();

$cid = 0;

go(function () use (&$cid)  {
    $fp = fopen(FILE, 'w+');
    Assert::true(flock($fp, LOCK_EX));
    Co::resume($cid);
    co::usleep(10000);
    flock($fp, LOCK_UN);
    fclose($fp);
});

go(function () use (&$cid) {
    $cid = Co::getCid();
    Co::yield();
    $fp = fopen(FILE, 'w+');
    Assert::same(flock($fp, LOCK_NB | LOCK_EX), false);
});

openswoole_event_wait();
unlink(FILE);
?>
--EXPECTF--
