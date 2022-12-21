--TEST--
swoole_coroutine: current cid
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Assert::same(Co::getCid(), -1);
go(function () {
    Assert::same(Co::getCid(), 1);
    Co::sleep(1);
    Assert::same(Co::getCid(), 1);
});
go(function () {
    Assert::same(Co::getCid(), 2);
});
Assert::same(Co::getCid(), -1);
?>
--EXPECT--
