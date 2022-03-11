--TEST--
swoole_redis_coro: redis lock
--CONFLICTS--
swoole_redis_coro
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    $redis = new Co\Redis();
    $redis->connect(REDIS_SERVER_HOST, REDIS_SERVER_PORT);
    $redis->delete('lock');
    $ret = $redis->set('lock', 1, ['nx', 'ex' => 1, 'px' => 1000]); // px will be ignored
    Assert::assert($ret);
    $ret = $redis->set('lock', 1, ['nx', 'ex' => 1, 'px' => 1000]); // px will be ignored
    Assert::assert(!$ret);
    $redis->delete('lock');
    $ret = $redis->set('lock', 1, ['nx', 'px' => 100]);
    Assert::assert($ret);
    usleep(50 * 1000);
    $ret = $redis->set('lock', 1, ['nx', 'px' => 100]);
    Assert::assert(!$ret);
    usleep(100 * 1000);
    $ret = $redis->set('lock', 1, ['nx', 'px' => 100]);
    Assert::assert($ret);
});
swoole_event_wait();
echo "DONE\n";
?>
--EXPECT--
DONE
