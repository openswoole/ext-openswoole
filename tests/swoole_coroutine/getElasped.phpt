--TEST--
swoole_coroutine: getElapsed
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co\run(function () {
    var_dump(Co::getElapsed(1000));
    var_dump(Co::getElapsed(-1));
    co::usleep(1000);
    var_dump(Co::getElapsed() === Co::getElapsed(Co::getCid()));
});

?>
--EXPECT--
int(-1)
int(-1)
bool(true)
