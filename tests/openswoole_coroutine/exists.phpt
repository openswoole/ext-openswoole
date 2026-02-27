--TEST--
openswoole_coroutine: exists
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    go(function () {
        go(function () {
            Co::usleep(1000);
            var_dump(Co::exists(Co::getPcid())); // 1: true
        });
        go(function () {
            Co::usleep(3000);
            var_dump(Co::exists(Co::getPcid())); // 3: false
        });
        co::usleep(2000);
        var_dump(Co::exists(Co::getPcid())); // 2: false
    });
});
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
