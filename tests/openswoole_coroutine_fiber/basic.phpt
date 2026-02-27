--TEST--
openswoole_coroutine_fiber: basic coroutine create and execute
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$result = [];

Co::set(['use_fiber_context' => true]);
co::run(function () use (&$result) {
    $result[] = 'start';

    go(function () use (&$result) {
        $result[] = 'coro1_start';
        $cid = Co::getCid();
        Assert::assert($cid > 0);
        $result[] = 'coro1_end';
    });

    go(function () use (&$result) {
        $result[] = 'coro2_start';
        $cid = Co::getCid();
        Assert::assert($cid > 0);
        $result[] = 'coro2_end';
    });

    $result[] = 'end';
});

Assert::same($result, ['start', 'coro1_start', 'coro1_end', 'coro2_start', 'coro2_end', 'end']);
echo "DONE\n";
?>
--EXPECT--
DONE
