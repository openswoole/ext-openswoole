--TEST--
swoole_coroutine: all asleep
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80100) die("Skipped: php version >= 8.1"); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set([
    'enable_deadlock_check' => true,
]);

function test1()
{
    $f = function () {
        Co::yield();
    };
    $f();
}

function test2()
{
    Co::yield();
}

Co\run(function () {
    go(function () {
        test1();
    });
    go(function () {
        test2();
    });
    co::usleep(100000);
});
echo "DONE\n";
?>
--EXPECTF--
===================================================================
 [FATAL ERROR]: all coroutines (count: 2) are asleep - deadlock!
===================================================================

 [Coroutine-3]
--------------------------------------------------------------------
#0 %s(%d): Swoole\Coroutine::yield()
#1 %s(%d): test2()
#2 [internal function]: {closure}()


 [Coroutine-2]
--------------------------------------------------------------------
#0 %s(%d): Swoole\Coroutine::yield()
#1 %s(%d): {closure}()
#2 %s(%d): test1()
#3 [internal function]: {closure}()

DONE
