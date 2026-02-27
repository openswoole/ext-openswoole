--TEST--
openswoole_global: deny create object
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
go(function () {
    try {
        new OpenSwoole\Coroutine;
    } catch (Error $e) {
        echo $e->getMessage() . PHP_EOL;
    }
    try {
        new OpenSwoole\Event;
    } catch (Error $e) {
        echo $e->getMessage() . PHP_EOL;
    }
    try {
        new OpenSwoole\Runtime;
    } catch (Error $e) {
        echo $e->getMessage() . PHP_EOL;
    }
    try {
        new OpenSwoole\Timer;
    } catch (Error $e) {
        echo $e->getMessage() . PHP_EOL;
    }
});
?>
--EXPECT--
The object of OpenSwoole\Coroutine can not be created for security reasons
The object of OpenSwoole\Event can not be created for security reasons
The object of OpenSwoole\Runtime can not be created for security reasons
The object of OpenSwoole\Timer can not be created for security reasons
