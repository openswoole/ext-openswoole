--TEST--
swoole_runtime: get hook flags
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Swoole\Runtime::enableCoroutine();
Assert::same(Swoole\Runtime::getHookFlags(), SWOOLE_HOOK_ALL);
?>
--EXPECT--
