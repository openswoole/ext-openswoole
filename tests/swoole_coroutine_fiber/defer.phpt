--TEST--
swoole_coroutine_fiber: defer tasks with fiber context
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$result = [];

Co::set(['use_fiber_context' => true]);
co::run(function () use (&$result) {
    go(function () use (&$result) {
        defer(function () use (&$result) {
            $result[] = 'defer2';
        });
        defer(function () use (&$result) {
            $result[] = 'defer1';
        });
        $result[] = 'coro_body';
    });
});

// defers execute in LIFO order
Assert::same($result, ['coro_body', 'defer1', 'defer2']);
echo "DONE\n";
?>
--EXPECT--
DONE
