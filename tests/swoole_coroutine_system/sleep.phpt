--TEST--
swoole_coroutine_system: sleep
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Swoole\Coroutine::create(function () {
    Swoole\Coroutine\System::usleep(500000);
    echo "OK";
});

?>
--EXPECT--
OK
