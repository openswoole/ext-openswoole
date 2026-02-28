--TEST--
openswoole_coroutine/cancel: wait/waitpid
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\System;

Co::run(function () {
    $cid = Coroutine::getCid();
    go(function () use ($cid) {
        System::usleep(2000);
        Assert::true(Coroutine::cancel($cid));
    });
    $retval = System::wait();
    echo "Done\n";
    Assert::eq($retval, false);
    Assert::eq(\OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANCELED);
});

?>
--EXPECT--
Done
