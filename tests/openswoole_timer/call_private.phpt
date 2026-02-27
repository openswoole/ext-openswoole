--TEST--
openswoole_timer: call private method
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80000) die("Skipped: php version >= 8."); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class Test
{
    private static function foo() { }

    private function bar() { }
}

openswoole_fork_exec(function () {
    OpenSwoole\Timer::After(1, [Test::class, 'not_exist']);
});
openswoole_fork_exec(function () {
    OpenSwoole\Timer::After(1, [Test::class, 'foo']);
});
openswoole_fork_exec(function () {
    OpenSwoole\Timer::After(1, [new Test, 'bar']);
});

?>
--EXPECTF--
Fatal error: Uncaught TypeError: OpenSwoole\Timer::after(): Argument #2 ($callback) must be a valid callback, class Test does not have a method "not_exist" in %s/tests/openswoole_timer/call_private.php:%d
Stack trace:
#0 %s/tests/openswoole_timer/call_private.php(%d): OpenSwoole\Timer::after(1, Array)
#1 [internal function]: {closure%S}(Object(OpenSwoole\Process))
#2 %s/tests/include/functions.php(%d): OpenSwoole\Process->start()
#3 %s/tests/openswoole_timer/call_private.php(%d): openswoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/openswoole_timer/call_private.php on line %d

Fatal error: Uncaught TypeError: OpenSwoole\Timer::after(): Argument #2 ($callback) must be a valid callback, cannot access private method Test::foo() in %s/tests/openswoole_timer/call_private.php:%d
Stack trace:
#0 %s/tests/openswoole_timer/call_private.php(%d): OpenSwoole\Timer::after(1, Array)
#1 [internal function]: {closure%S}(Object(OpenSwoole\Process))
#2 %s/tests/include/functions.php(%d): OpenSwoole\Process->start()
#3 %s/tests/openswoole_timer/call_private.php(%d): openswoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/openswoole_timer/call_private.php on line %d

Fatal error: Uncaught TypeError: OpenSwoole\Timer::after(): Argument #2 ($callback) must be a valid callback, cannot access private method Test::bar() in %s/tests/openswoole_timer/call_private.php:%d
Stack trace:
#0 %s/tests/openswoole_timer/call_private.php(%d): OpenSwoole\Timer::after(1, Array)
#1 [internal function]: {closure%S}(Object(OpenSwoole\Process))
#2 %s/tests/include/functions.php(%d): OpenSwoole\Process->start()
#3 %s/tests/openswoole_timer/call_private.php(%d): openswoole_fork_exec(Object(Closure))
#4 {main}
  thrown in %s/tests/openswoole_timer/call_private.php on line %d
