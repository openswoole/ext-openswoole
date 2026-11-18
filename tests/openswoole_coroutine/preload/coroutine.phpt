--TEST--
openswoole_coroutine/preload: coroutines with opcache.preload
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
if (!extension_loaded('Zend OPcache')) {
    skip('opcache is not loaded');
}
?>
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.preload={PWD}/empty.inc
opcache.preload_user=root
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

co::run(function () {
    co::usleep(1000);
    go(function () {
        co::usleep(1000);
        echo "child coroutine resumed\n";
    });
    echo "main coroutine resumed\n";
});
echo "done\n";
?>
--EXPECT--
main coroutine resumed
child coroutine resumed
done
