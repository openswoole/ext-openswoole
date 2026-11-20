--TEST--
openswoole_lock: methods after destroy
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;

$types = [Lock::MUTEX];
if (defined('OpenSwoole\Lock::RWLOCK')) {
    $types[] = Lock::RWLOCK;
}
if (defined('OpenSwoole\Lock::SPINLOCK')) {
    $types[] = Lock::SPINLOCK;
}
foreach ($types as $type) {
    $lock = new Lock($type);
    Assert::true($lock->lock());
    Assert::true($lock->unlock());
    Assert::null($lock->destroy());
    unset($lock);
}

$lock = new Lock();
$lock->destroy();
echo "destroyed\n";
$lock->lock();
echo "unreachable\n";
?>
--EXPECTF--
destroyed

Fatal error: OpenSwoole\Lock::lock(): you must call Lock constructor first in %s on line %d%A
