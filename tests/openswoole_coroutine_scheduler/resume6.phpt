--TEST--
openswoole_coroutine_scheduler: user yield and resume4
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine as co;

go(function () {
    echo "coro 1 start\n";
    co::yield();
    echo "coro 1 end\n";
});
echo "main 1\n";
go(function () {
    echo "coro 2 start\n";
    co::resume(1);
    echo "coro 2 end\n";
});
echo "main 2\n";
openswoole_event::wait();
?>
--EXPECTF--
coro 1 start
main 1
coro 2 start
coro 1 end
coro 2 end
main 2
