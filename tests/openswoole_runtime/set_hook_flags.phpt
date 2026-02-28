--TEST--
openswoole_runtime: set hook flags
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Runtime;

Runtime::setHookFlags(SWOOLE_HOOK_ALL);

$load = function ($name) {
    $flags = Runtime::getHookFlags();
    Runtime::setHookFlags(0);
    usleep(10000);
    eval('class InexistentClass{}');
    Runtime::setHookFlags($flags);
};

spl_autoload_register($load);
co::run(function () {

    go(function () {
        new InexistentClass;
    });
    go(function () {
        new InexistentClass;
    });
});

?>
--EXPECT--
