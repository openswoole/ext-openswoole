--TEST--
openswoole_coroutine_fiber: coroutine stats with fiber context
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['use_fiber_context' => true]);
Assert::same(Co::stats()['coroutine_num'], 0);
Assert::same(Co::stats()['coroutine_peak_num'], 0);
go(function () {
    Assert::same(Co::stats()['coroutine_num'], 1);
    Assert::same(Co::stats()['coroutine_peak_num'], 1);
    co::usleep(500000);
    Assert::same(Co::stats()['coroutine_num'], 2);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
});
go(function () {
    Assert::same(Co::stats()['coroutine_num'], 2);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
    co::usleep(500000);
    Assert::same(Co::stats()['coroutine_num'], 1);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
});
Assert::same(Co::stats()['coroutine_num'], 2);
Assert::same(Co::stats()['coroutine_peak_num'], 2);
echo "DONE\n";
?>
--EXPECT--
DONE
