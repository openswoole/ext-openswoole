--TEST--
openswoole_lock: rwlock shares read locks between processes and excludes writers
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
if (!defined('OpenSwoole\Lock::RWLOCK')) {
    skip('rwlock is not supported');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;
use OpenSwoole\Process;

$lock = new Lock(Lock::RWLOCK);

// readers in the same process share the lock, a writer cannot take it
Assert::true($lock->lock_read());
Assert::true($lock->trylock_read());
Assert::false($lock->trylock());
Assert::same($lock->errCode, SOCKET_EBUSY);
Assert::true($lock->unlock());
Assert::true($lock->unlock());

// read lock held by the parent: another process can read but not write
Assert::true($lock->lock_read());
$child = new Process(function (Process $w) use ($lock) {
    $read = $lock->trylock_read();
    if ($read) {
        $lock->unlock();
    }
    $write = $lock->trylock();
    $w->write(json_encode([$read, $write, $lock->errCode]));
});
$child->start();
[$read, $write, $errCode] = json_decode($child->read(), true);
Process::wait();
Assert::true($read);
Assert::false($write);
Assert::same($errCode, SOCKET_EBUSY);

// a blocking writer waits until the last reader unlocks
$child = new Process(function (Process $w) use ($lock) {
    $w->write('ready');
    $start = microtime(true);
    $ok = $lock->lock();
    $elapsed = microtime(true) - $start;
    $lock->unlock();
    $w->write(json_encode([$ok, $elapsed]));
});
$child->start();
Assert::same($child->read(), 'ready');
usleep(300000);
Assert::true($lock->unlock());
[$ok, $elapsed] = json_decode($child->read(), true);
Process::wait();
Assert::true($ok);
Assert::greaterThanEq($elapsed, 0.2);

// write lock held by a child: the parent can neither read nor write
$child = new Process(function (Process $w) use ($lock) {
    $lock->lock();
    $w->write('locked');
    $w->read();
    $lock->unlock();
    $w->write('unlocked');
});
$child->start();
Assert::same($child->read(), 'locked');
Assert::false($lock->trylock_read());
Assert::same($lock->errCode, SOCKET_EBUSY);
Assert::false($lock->trylock());
$child->write('go');
Assert::same($child->read(), 'unlocked');
Process::wait();
Assert::true($lock->trylock_read());
Assert::true($lock->unlock());
Assert::true($lock->trylock());
Assert::true($lock->unlock());
echo "DONE\n";
?>
--EXPECT--
DONE
