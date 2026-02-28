--TEST--
openswoole_coroutine: user coroutine
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
OpenSwoole\Coroutine::create(function ()
{
    echo "OK\n";
});

?>
--EXPECT--
OK
