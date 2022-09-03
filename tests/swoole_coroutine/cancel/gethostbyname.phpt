--TEST--
swoole_coroutine/cancel: gethostbyname
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



use Swoole\Coroutine;
use Swoole\Event;
use Swoole\Coroutine\System;

Co::run(function () {
    $cid = Coroutine::getCid();
    Event::defer(function () use ($cid) {
        Assert::true(Coroutine::cancel($cid));
    });
    $retval = System::gethostbyname('www.baidu.com');
    echo "Done\n";
    Assert::eq($retval, false);
    Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_CO_CANCELED);
});

?>
--EXPECT--
Done
