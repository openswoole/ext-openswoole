--TEST--
openswoole_coroutine/bailout: error
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
go(function () {
    go(function () {
        $a = str_repeat('A', 1024 * 1024 * 1024 * 1024);
        co::usleep(100000);
    });
});
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s%A
