--TEST--
swoole_coroutine: coro nested strict
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::same(Co::getCid(), -1);
go(function () {
    Assert::same(Co::getCid(), 1);
    co::usleep(10000);
    Assert::same(Co::getCid(), 1);
});
Assert::same(Co::getCid(), -1);
go(function () {
    Assert::same(Co::getCid(), 2);

    go(function () {
        Assert::same(Co::getCid(), 3);
        go(function () {
            Assert::same(Co::getCid(), 4);
            go(function () {
                Assert::same(Co::getCid(), 5);
                co::usleep(10000);
                Assert::same(Co::getCid(), 5);
            });
            Assert::same(Co::getCid(), 4);
        });
        Assert::same(Co::getCid(), 3);
    });
    Assert::same(Co::getCid(), 2);
});
Assert::same(Co::getCid(), -1);
?>
--EXPECT--
