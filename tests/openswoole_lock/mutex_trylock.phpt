--TEST--
openswoole_lock: mutex trylock across processes
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;
use OpenSwoole\Process;

$lock = new Lock(Lock::MUTEX);
Assert::same($lock->errCode, 0);

Assert::true($lock->trylock());
Assert::false($lock->trylock());
Assert::same($lock->errCode, SOCKET_EBUSY);
// a read lock on a mutex is the same lock
Assert::false($lock->trylock_read());
Assert::true($lock->unlock());
Assert::true($lock->lock_read());
Assert::false($lock->trylock());
Assert::true($lock->unlock());

// held by the parent: a child fails to trylock, then waits in lockwait
Assert::true($lock->lock());
$child = new Process(function (Process $w) use ($lock) {
    $try = $lock->trylock();
    $tryErr = $lock->errCode;
    $start = microtime(true);
    $wait = $lock->lockwait(0.1);
    $waitErr = $lock->errCode;
    $timedOut = microtime(true) - $start;
    $w->write('ready');
    $start = microtime(true);
    $ok = $lock->lockwait(5);
    $elapsed = microtime(true) - $start;
    $lock->unlock();
    $w->write(json_encode([$try, $tryErr, $wait, $waitErr, $timedOut, $ok, $elapsed]));
});
$child->start();
Assert::same($child->read(), 'ready');
usleep(200000);
Assert::true($lock->unlock());
[$try, $tryErr, $wait, $waitErr, $timedOut, $ok, $elapsed] = json_decode($child->read(), true);
Process::wait();
Assert::false($try);
Assert::same($tryErr, SOCKET_EBUSY);
Assert::false($wait);
Assert::same($waitErr, SOCKET_ETIMEDOUT);
Assert::greaterThanEq($timedOut, 0.05);
Assert::true($ok);
Assert::greaterThanEq($elapsed, 0.1);
Assert::lessThan($elapsed, 5);

// released by the child: the parent takes it again
Assert::true($lock->trylock());
Assert::true($lock->unlock());
echo "DONE\n";
?>
--EXPECT--
DONE
