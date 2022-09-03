--TEST--
swoole_runtime: set hook flags
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['hook_flags' => 0]);

co::run(function () {
    Assert::eq(Swoole\Runtime::getHookFlags(), 0);
});

Co::set(['hook_flags' => SWOOLE_HOOK_CURL]);

co::run(function () {
    Assert::eq(Swoole\Runtime::getHookFlags(), SWOOLE_HOOK_CURL);
});

?>
--EXPECT--
