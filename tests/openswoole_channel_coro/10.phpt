--TEST--
openswoole_channel_coro: 10
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    $chan = new chan(3);
    go(function () use ($chan) {
        Co::usleep(1000);
        $chan->push("data");
    });
    Assert::same($chan->pop(0.001), "data");
    Assert::false($chan->pop(0.001));
});

openswoole_event::wait();
?>
--EXPECT--
