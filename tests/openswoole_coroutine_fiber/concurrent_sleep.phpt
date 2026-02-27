--TEST--
openswoole_coroutine_fiber: concurrent coroutines with sleep
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$count = 0;

Co::set(['use_fiber_context' => true]);
co::run(function () use (&$count) {
    for ($i = 0; $i < 10; $i++) {
        go(function () use (&$count, $i) {
            co::usleep(1000 * ($i + 1));
            $count++;
        });
    }
});

Assert::same($count, 10);
echo "DONE\n";
?>
--EXPECT--
DONE
