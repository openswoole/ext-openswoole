--TEST--
swoole_runtime: enableCoroutine
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Swoole\Runtime::enableCoroutine(SWOOLE_HOOK_TCP);

co::run(function () {
    Assert::eq(Swoole\Runtime::getHookFlags(), SWOOLE_HOOK_TCP);
});

co::run(function () {
    Assert::eq(Swoole\Runtime::getHookFlags(), SWOOLE_HOOK_TCP);
});

?>
--EXPECT--
