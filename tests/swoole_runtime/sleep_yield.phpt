--TEST--
swoole_runtime: sleep yield
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Swoole\Runtime::enableCoroutine();
Co\run(function () {
    $timer1 = Swoole\Timer::tick(1000, function () {
        var_dump('a');
    });
    go(function () use ($timer1) {
        for ($i = 0; $i < 10; ++$i) {
            Co::usleep(100000);
        }
        var_dump('b');
        Swoole\Timer::clear($timer1);
    });
});
?>
--EXPECTF--
string(1) "a"
string(1) "b"
