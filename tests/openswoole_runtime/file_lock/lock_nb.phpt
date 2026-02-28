--TEST--
openswoole_runtime/file_lock: lock_nb
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80500) die("Skipped: flock LOCK_NB behavior changed in PHP 8.5"); ?>
<?php
$f = tempnam(sys_get_temp_dir(), 'lk');
$fp1 = fopen($f, 'w+'); flock($fp1, LOCK_EX);
$fp2 = fopen($f, 'w+'); $r = flock($fp2, LOCK_EX | LOCK_NB);
fclose($fp1); fclose($fp2); unlink($f);
if ($r !== false) die("skip: LOCK_NB not supported in this environment");
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
const FILE = __DIR__ . '/test.data';
\OpenSwoole\Runtime::enableCoroutine();
go(function () {
    $fp = fopen(FILE, 'w+');
    Assert::assert(flock($fp, LOCK_EX));
    $fp2 = fopen(FILE, 'w+');
    Assert::assert(!flock($fp2, LOCK_EX | LOCK_NB));
});
openswoole_event_wait();
unlink(FILE);
?>
--EXPECTF--
