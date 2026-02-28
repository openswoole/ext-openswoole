--TEST--
openswoole_runtime: enableCoroutine
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(SWOOLE_HOOK_TCP);

co::run(function () {
    Assert::eq(OpenSwoole\Runtime::getHookFlags(), SWOOLE_HOOK_TCP);
});

co::run(function () {
    Assert::eq(OpenSwoole\Runtime::getHookFlags(), SWOOLE_HOOK_TCP);
});

?>
--EXPECT--
