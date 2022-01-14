--TEST--
swoole_coroutine/forbidden_case: coro invoke
--SKIPIF--
<?php require  __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Coroutine as co;
co::create(function() {
    $function = new ReflectionFunction('foo');
    $function->invoke();
    echo "co end\n";
});
function foo() {
   echo "func start\n";
   co::usleep(1000);
   echo "func end\n";
}
echo "main end\n";
?>
--EXPECT--
func start
main end
func end
co end
