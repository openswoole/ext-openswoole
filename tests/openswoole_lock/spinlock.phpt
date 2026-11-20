--TEST--
openswoole_lock: spinlock lock, trylock and unlock across processes
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
if (!defined('OpenSwoole\Lock::SPINLOCK')) {
    skip('spinlock is not supported');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;
use OpenSwoole\Process;

$lock = new Lock(Lock::SPINLOCK);

Assert::true($lock->trylock());
Assert::false($lock->trylock());
Assert::same($lock->errCode, SOCKET_EBUSY);
// read locks are plain locks on a spinlock
Assert::false($lock->trylock_read());
Assert::true($lock->unlock());
Assert::true($lock->trylock_read());
Assert::true($lock->unlock());

// held by the parent: a child cannot take it until it is released
Assert::true($lock->lock());
$child = new Process(function (Process $w) use ($lock) {
    $busy = !$lock->trylock();
    $w->write('ready');
    $start = microtime(true);
    $ok = $lock->lock();
    $elapsed = microtime(true) - $start;
    $lock->unlock();
    $w->write(json_encode([$busy, $ok, $elapsed]));
});
$child->start();
Assert::same($child->read(), 'ready');
usleep(200000);
Assert::true($lock->unlock());
[$busy, $ok, $elapsed] = json_decode($child->read(), true);
Process::wait();
Assert::true($busy);
Assert::true($ok);
Assert::greaterThanEq($elapsed, 0.1);

// held by a child: the parent cannot take it
$child = new Process(function (Process $w) use ($lock) {
    $lock->lock();
    $w->write('locked');
    $w->read();
    $lock->unlock();
    $w->write('unlocked');
});
$child->start();
Assert::same($child->read(), 'locked');
Assert::false($lock->trylock());
$child->write('go');
Assert::same($child->read(), 'unlocked');
Process::wait();
Assert::true($lock->trylock());
Assert::true($lock->unlock());

// lockwait is only available on a mutex
Assert::throws(function () use ($lock) {
    $lock->lockwait(0.1);
}, OpenSwoole\Exception::class, 'only mutex supports lockwait');
echo "DONE\n";
?>
--EXPECT--
DONE
