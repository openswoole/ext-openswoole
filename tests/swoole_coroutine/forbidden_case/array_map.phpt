--TEST--
swoole_coroutine/forbidden_case: coro array map
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Coroutine as co;
co::create(function() {
    array_map("test",array("func start\n"));
    echo "co end\n";
});
function test($p) {
    echo $p;
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
