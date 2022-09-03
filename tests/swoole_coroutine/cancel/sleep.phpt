--TEST--
swoole_coroutine/cancel: sleep
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use Swoole\Coroutine;
use Swoole\Coroutine\System;

Co::run(function () {
    $cid = Coroutine::getCid();
    go(function () use ($cid) {
        System::usleep(2000);
        Assert::true(Coroutine::cancel($cid));
        Assert::false(Coroutine::isCanceled());
    });
    $retval = System::sleep(10000);
    echo "Done\n";
    Assert::eq($retval, false);
    Assert::assert(Coroutine::isCanceled());
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANCELED);
});

?>
--EXPECT--
Done
