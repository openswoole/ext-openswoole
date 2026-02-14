--TEST--
swoole_runtime/file_lock: lock_nb
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80500) die("Skipped: flock LOCK_NB behavior changed in PHP 8.5"); ?>
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
swoole_event_wait();
unlink(FILE);
?>
--EXPECTF--
