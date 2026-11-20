--TEST--
openswoole_lock: destroy() in a child process keeps the lock usable for its creator
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_function_not_exist('pcntl_fork');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Lock;

foreach ([Lock::MUTEX, Lock::RWLOCK, Lock::SPINLOCK] as $type) {
    $lock = new Lock($type);
    $pid = pcntl_fork();
    if ($pid === 0) {
        $lock->destroy();
        exit(0);
    }
    pcntl_waitpid($pid, $status);
    Assert::true($lock->lock());
    Assert::true($lock->unlock());
    Assert::true($lock->trylock());
    Assert::true($lock->unlock());
}
echo "DONE\n";
?>
--EXPECT--
DONE
