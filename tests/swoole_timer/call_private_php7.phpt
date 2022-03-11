--TEST--
swoole_timer: call private method
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80000) die("Skipped: php version < 8."); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class Test
{
    private static function foo() { }

    private function bar() { }
}

swoole_fork_exec(function () {
    Swoole\Timer::After(1, [Test::class, 'not_exist']);
});
swoole_fork_exec(function () {
    Swoole\Timer::After(1, [Test::class, 'foo']);
});
swoole_fork_exec(function () {
    Swoole\Timer::After(1, [new Test, 'bar']);
});

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Argument 2 passed to Swoole\Timer::after() must be callable, array given in %s/tests/swoole_timer/call_private_php7.php:%d
Stack trace:
#0 %s/tests/swoole_timer/call_private_php7.php(%d): Swoole\Timer::after(1, Array)
#1 [internal function]: {closure}(Object(Swoole\Process))
#2 %s/tests/include/functions.php(%d): Swoole\Process->start()
#3 %s/tests/swoole_timer/call_private_php7.php(%d): swoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/swoole_timer/call_private_php7.php on line %d

Fatal error: Uncaught TypeError: Argument 2 passed to Swoole\Timer::after() must be callable, array given in %s/tests/swoole_timer/call_private_php7.php:%d
Stack trace:
#0 %s/tests/swoole_timer/call_private_php7.php(%d): Swoole\Timer::after(1, Array)
#1 [internal function]: {closure}(Object(Swoole\Process))
#2 %s/tests/include/functions.php(%d): Swoole\Process->start()
#3 %s/tests/swoole_timer/call_private_php7.php(%d): swoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/swoole_timer/call_private_php7.php on line %d

Fatal error: Uncaught TypeError: Argument 2 passed to Swoole\Timer::after() must be callable, array given in %s/tests/swoole_timer/call_private_php7.php:%d
Stack trace:
#0 %s/tests/swoole_timer/call_private_php7.php(%d): Swoole\Timer::after(1, Array)
#1 [internal function]: {closure}(Object(Swoole\Process))
#2 %s/tests/include/functions.php(%d): Swoole\Process->start()
#3 %s/tests/swoole_timer/call_private_php7.php(%d): swoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/swoole_timer/call_private_php7.php on line %d
