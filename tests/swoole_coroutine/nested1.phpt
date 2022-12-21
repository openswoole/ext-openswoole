--TEST--
swoole_coroutine: coro channel
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    echo "co[1] start\n";
    co::usleep(10000);
    go(function () {
        echo "co[2] start\n";
        co::usleep(10000);
        echo "co[2] exit\n";
    });
    echo "co[1] exit\n";
});
?>
--EXPECT--
co[1] start
co[2] start
co[1] exit
co[2] exit
