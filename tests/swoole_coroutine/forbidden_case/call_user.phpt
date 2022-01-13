--TEST--
swoole_coroutine/forbidden_case: coro call user func
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Coroutine as co;
co::create(function() {
    $name = "call_user_func";
    $return = $name("test");
    echo "co end\n";
});

function test() {
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
