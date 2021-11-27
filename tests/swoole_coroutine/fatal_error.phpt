--TEST--
swoole_coroutine: fatal error
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set([
    'enable_deadlock_check' => true,
]);


Co\run(function () {
    test_not_found();
});
echo "DONE\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function test_not_found() in %s:%d
Stack trace:
%A
  thrown in %s on line %d
