--TEST--
swoole_redis_coro: redis unconnected recv
--CONFLICTS--
swoole_redis_coro
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $redis = new Swoole\Coroutine\Redis;
    $redis->setDefer(true);
    Assert::false($redis->recv());
    echo "DONE\n";
});
?>
--EXPECT--
DONE
