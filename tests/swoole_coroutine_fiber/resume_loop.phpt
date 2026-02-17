--TEST--
swoole_coroutine_fiber: resume loop with many coroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Co::set(['use_fiber_context' => true]);
$cos = [];
for ($n = 500; $n--;) {
    $cos[] = go(function () {
        global $cos;
        Co::yield();
        if (count($cos) > 0) {
            Co::resume(array_shift($cos));
        }
    });
}
Co::resume(array_shift($cos));
echo "DONE\n";
?>
--EXPECT--
DONE
