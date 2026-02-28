--TEST--
openswoole_runtime: set hook flags
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    co::set(['hook_flags' => 0]);
    Assert::eq(OpenSwoole\Runtime::getHookFlags(), 0);
    Co::set(['hook_flags' => OpenSwoole\Runtime::HOOK_CURL]);
    Assert::eq(OpenSwoole\Runtime::getHookFlags(), OpenSwoole\Runtime::HOOK_CURL);
});

?>
--EXPECT--
