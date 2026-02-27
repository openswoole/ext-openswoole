--TEST--
openswoole_coroutine: coro parallel3
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    echo "co[1] start\n";
    co::usleep(20000);
    echo "co[1] exit\n";
});

go(function () {
    echo "co[2] start\n";
    go(function () {
        echo "co[3] start\n";
        co::usleep(30000);
        echo "co[3] exit\n";
    });
    co::usleep(10000);
    echo "co[2] exit\n";
});
echo "end\n";
OpenSwoole\Event::wait();
?>
--EXPECT--
co[1] start
co[2] start
co[3] start
end
co[2] exit
co[1] exit
co[3] exit
