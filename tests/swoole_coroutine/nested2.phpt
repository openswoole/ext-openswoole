--TEST--
swoole_coroutine: coro nested2
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    echo "co[1] start\n";
    go(function () {
        echo "co[2] start\n";
        co::usleep(20000);
        echo "co[2] exit\n";
    });
    co::usleep(10000);
    echo "co[1] exit\n";
});
echo "end\n";
?>
--EXPECT--
co[1] start
co[2] start
end
co[1] exit
co[2] exit
