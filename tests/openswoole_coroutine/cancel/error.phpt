--TEST--
openswoole_coroutine/cancel: error
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\System;

Co::run(function () {
    Assert::false(Coroutine::cancel(Coroutine::getCid()));
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANNOT_CANCEL);
    
    Assert::false(Coroutine::cancel(999));
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_NOT_EXISTS);
});

?>
--EXPECT--
