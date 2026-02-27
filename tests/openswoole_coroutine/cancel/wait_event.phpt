--TEST--
openswoole_coroutine/cancel: waitEvent
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\System;

Co::run(function () {
    $fp = stream_socket_client('tcp://www.baidu.com:80/', $errno, $errmsg, 1);
    Assert::assert($fp);

    $cid = Coroutine::getCid();
    go(function () use ($cid) {
        System::usleep(2000);
        Assert::true(Coroutine::cancel($cid));
    });

    $retval = System::waitEvent($fp);
    echo "Done\n";
    Assert::eq($retval, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANCELED);
});

?>
--EXPECT--
Done
