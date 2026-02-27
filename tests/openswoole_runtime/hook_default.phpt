--TEST--
openswoole_runtime: enable hook by default
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    Assert::eq(OpenSwoole\Runtime::getHookFlags(), SWOOLE_HOOK_ALL);
});

?>
--EXPECT--
