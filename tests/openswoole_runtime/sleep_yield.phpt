--TEST--
openswoole_runtime: sleep yield
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
OpenSwoole\Runtime::enableCoroutine();
co::run(function () {
    $timer1 = OpenSwoole\Timer::tick(1000, function () {
        var_dump('a');
    });
    go(function () use ($timer1) {
        for ($i = 0; $i < 10; ++$i) {
            Co::usleep(100000);
        }
        var_dump('b');
        OpenSwoole\Timer::clear($timer1);
    });
});
?>
--EXPECTF--
string(1) "a"
string(1) "b"
