--TEST--
openswoole_backward_compat: coroutine via swoole_coroutine_create and Swoole\Coroutine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Test swoole_coroutine_create function alias
$result = null;
Swoole\Coroutine\run(function () use (&$result) {
    swoole_coroutine_create(function () use (&$result) {
        $result = Swoole\Coroutine::getCid();
        Assert::greaterThan($result, 0);
    });
});
Assert::greaterThan($result, 0);

// Test go() shorthand
$result2 = null;
Co\run(function () use (&$result2) {
    go(function () use (&$result2) {
        $result2 = Co::getCid();
        Assert::greaterThan($result2, 0);
    });
});
Assert::greaterThan($result2, 0);

// Test defer via swoole_coroutine_defer
$deferred = false;
Co\run(function () use (&$deferred) {
    swoole_coroutine_defer(function () use (&$deferred) {
        $deferred = true;
    });
});
Assert::assert($deferred);

echo "PASSED\n";
?>
--EXPECT--
PASSED
