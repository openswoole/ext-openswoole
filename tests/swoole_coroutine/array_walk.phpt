--TEST--
swoole_coroutine: array_walk
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    for ($n = 2; $n--;) {
        go(function () {
            $array = range(0, 1);
            array_walk($array, function ($item) {
                Co::usleep([10000, 1000][$item]);
            });
        });
    }
});
echo "DONE\n";
?>
--EXPECTF--
DONE
