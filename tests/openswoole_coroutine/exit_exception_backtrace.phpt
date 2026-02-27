--TEST--
openswoole_coroutine: exit exception backtrace
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80400) die("Skipped"); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
function foo()
{
    bar(get_safe_random());
}

function bar(string $random)
{
    char(mt_rand(0, PHP_INT_MAX));
}

function char(int $random)
{
    exit;
}

co::run(function() {
    go(function () {
        Co::usleep(1000);
    });
    go(function () {
        foo();
    });
});


?>
--EXPECTF--
Fatal error: Uncaught OpenSwoole\ExitException: swoole exit in %s/tests/openswoole_coroutine/exit_exception_backtrace.php:15
Stack trace:
#0 %s/tests/openswoole_coroutine/exit_exception_backtrace.php(10): char(%d)
#1 %s/tests/openswoole_coroutine/exit_exception_backtrace.php(5): bar('%s...')
#2 %s/tests/openswoole_coroutine/exit_exception_backtrace.php(23): foo()
%A
  thrown in %s/tests/openswoole_coroutine/exit_exception_backtrace.php on line 15
