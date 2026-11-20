--TEST--
openswoole_lock: lockwait uses the default timeout without an argument
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$lock = new OpenSwoole\Lock(OpenSwoole\Lock::MUTEX);
Assert::true($lock->lockwait());
$start = microtime(true);
Assert::false($lock->lockwait());
$elapsed = microtime(true) - $start;
Assert::same($lock->errCode, SOCKET_ETIMEDOUT);
Assert::greaterThanEq($elapsed, 0.9);
Assert::lessThan($elapsed, 2);
Assert::true($lock->unlock());
echo "DONE\n";
?>
--EXPECT--
DONE
