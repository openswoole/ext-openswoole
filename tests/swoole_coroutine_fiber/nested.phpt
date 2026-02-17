--TEST--
swoole_coroutine_fiber: nested coroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (strpos($info, 'enabled with fiber context') === false) {
    die('skip fiber context not enabled');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    echo "co[1] start\n";
    co::usleep(10000);
    go(function () {
        echo "co[2] start\n";
        go(function () {
            echo "co[3] start\n";
            co::usleep(10000);
            echo "co[3] end\n";
        });
        co::usleep(20000);
        echo "co[2] end\n";
    });
    echo "co[1] end\n";
});
?>
--EXPECT--
co[1] start
co[2] start
co[3] start
co[1] end
co[3] end
co[2] end
