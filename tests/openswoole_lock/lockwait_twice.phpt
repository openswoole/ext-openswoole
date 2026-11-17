--TEST--
openswoole_lock: test lock twice
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$lock = new OpenSwoole\Lock();
var_dump($lock->lock());


$start = microtime(true);
$ret = $lock->lockwait(0.2);
Assert::false($ret);
$end = microtime(true);

Assert::eq($lock->errCode, SOCKET_ETIMEDOUT);
Assert::greaterThanEq($end - $start, 0.15);
Assert::lessThan($end - $start, 0.5);

?>
--EXPECT--
bool(true)
