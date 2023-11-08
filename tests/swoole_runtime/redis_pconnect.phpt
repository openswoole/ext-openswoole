--TEST--
swoole_runtime: hook stream redis pconnect
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_class_not_exist('Redis');
skip('TODOv23');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Swoole\Runtime::enableCoroutine();
co::run(function() {
    go(function () {
        $redis = new Redis;
        Assert::assert($redis->pconnect(REDIS_SERVER_HOST, REDIS_SERVER_PORT));
        $redis->get("key");
    });
    go(function () {
        $redis = new Redis;
        Assert::assert($redis->pconnect(REDIS_SERVER_HOST, REDIS_SERVER_PORT));
        $redis->get("key");
    });
});
echo "DONE\n";
?>
--EXPECT--
DONE
