--TEST--
openswoole_channel_coro: push timeout 3
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$channel = new OpenSwoole\Coroutine\Channel(1);

go(function () use ($channel) {
    Assert::assert($channel->push(1, 0.1));
    Assert::assert(!$channel->push(1, 0.1));
});

openswoole_event_wait();
echo "DONE\n";
?>
--EXPECT--
DONE
