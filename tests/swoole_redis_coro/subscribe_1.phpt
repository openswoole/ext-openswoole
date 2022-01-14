--TEST--
swoole_redis_coro: redis subscribe 1
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    $redis = new Co\Redis();
    $redis->connect(REDIS_SERVER_HOST, REDIS_SERVER_PORT);
    $val = $redis->subscribe(['test']);
    Assert::assert($val);

    $val = $redis->recv();
    Assert::assert($val[0] == 'subscribe' && $val[1] == 'test');

    for ($i = 0; $i < MAX_REQUESTS; $i++) {
        $val = $redis->recv();
        Assert::same($val[0] ?? '', 'message');
    }

    $redis->close();
});

go(function () {
    $redis = new Co\redis;
    $redis->connect(REDIS_SERVER_HOST, REDIS_SERVER_PORT);
    co::usleep(100000);

    for ($i = 0; $i < MAX_REQUESTS; $i++) {
        $ret = $redis->publish('test', 'hello-' . $i);
        Assert::assert($ret);
    }
});

?>
--EXPECT--
