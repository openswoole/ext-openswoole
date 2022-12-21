--TEST--
swoole_global: deny serialize and unserialize
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    try {
        $hcc = new \OpenSwoole\Atomic();
        serialize($hcc);
        Assert::true(false, 'never here');
    } catch (\Exception $exception) {
        Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
    }
    try {
        $hcc = new \OpenSwoole\Client(SWOOLE_TCP);
        serialize($hcc);
        Assert::true(false, 'never here');
    } catch (\Exception $exception) {
        Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
    }
    try {
        $hcc = new \OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        serialize($hcc);
        Assert::true(false, 'never here');
    } catch (\Exception $exception) {
        Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
    }
    try {
        $hcc = new \OpenSwoole\Coroutine\Http\Client('127.0.0.1');
        serialize($hcc);
        Assert::true(false, 'never here');
    } catch (\Exception $exception) {
        Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
    }
    if (HAS_ASYNC_REDIS) {
        try {
            $hcc = new \OpenSwoole\Coroutine\Redis();
            serialize($hcc);
            Assert::true(false, 'never here');
        } catch (\Exception $exception) {
            Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
        }
    }
    try {
        $hcc = new \OpenSwoole\Table(1);
        serialize($hcc);
        Assert::true(false, 'never here');
    } catch (\Exception $exception) {
        Assert::same(strpos($exception->getMessage(), 'Serialization'), 0);
    }
});
?>
--EXPECT--
