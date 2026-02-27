--TEST--
openswoole_coroutine/cancel: pop from channel
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\System;

Co::run(function () {
    $chan = new Coroutine\Channel(4);
    $cid = Coroutine::getCid();
    go(function () use ($cid) {
        System::usleep(2000);
        Assert::true(Coroutine::cancel($cid));
    });
    Assert::eq($chan->pop(100), false);
    Assert::assert(Coroutine::isCanceled());
    Assert::eq($chan->errCode, SWOOLE_CHANNEL_CANCELED);
    echo "Done\n";
});

?>
--EXPECT--
Done
