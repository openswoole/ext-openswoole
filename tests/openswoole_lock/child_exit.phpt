--TEST--
openswoole_lock: a lock stays usable in the parent after a child process exits
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_function_not_exist('pcntl_fork');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;
use OpenSwoole\Process;

$types = ['mutex' => Lock::MUTEX];
if (defined('OpenSwoole\Lock::RWLOCK')) {
    $types['rwlock'] = Lock::RWLOCK;
}
if (defined('OpenSwoole\Lock::SPINLOCK')) {
    $types['spinlock'] = Lock::SPINLOCK;
}

foreach ($types as $name => $type) {
    $lock = new Lock($type);

    $child = new Process(function () use ($lock) {
        $lock->lock();
        $lock->unlock();
    });
    $child->start();
    Process::wait();
    Assert::true($lock->trylock(), "$name after process exit");
    Assert::true($lock->unlock());

    $pid = pcntl_fork();
    if ($pid === 0) {
        $lock->lock();
        $lock->unlock();
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    Assert::true($lock->trylock(), "$name after fork exit");
    Assert::same($lock->errCode, 0);
    Assert::true($lock->unlock());
}
echo "DONE\n";
?>
--EXPECT--
DONE
