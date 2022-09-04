--TEST--
swoole_coroutine/output: use ob_* in nest co
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
go(function () {
    ob_start();
    echo "3\n"; // [#1] yield
    go(function () {
        echo "1\n"; // [#2] output: 1
        fgets(fopen(__FILE__, 'r')); // yield
        // [#4] resume
        ob_start(); // to buffer
        echo "2\n";
    }); // [#5] destroyed and output: 4
    echo "4\n";
}); // [#3] destroyed and output: 2 3
swoole_event_wait();
?>
--EXPECT--
1
2
3
4
