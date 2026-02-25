--TEST--
swoole_io_uring: concurrent file I/O from multiple coroutines
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$prefix = '/tmp/swoole_io_uring_concurrent_' . getmypid();

Co\run(function () use ($prefix) {
    $n = 10;
    $wg = new Co\WaitGroup();

    for ($i = 0; $i < $n; $i++) {
        $wg->add();
        go(function () use ($prefix, $i, $wg) {
            $file = "{$prefix}_{$i}";
            $data = "coroutine {$i} data: " . str_repeat('x', 1024);

            // Write
            file_put_contents($file, $data);

            // Read back
            $content = file_get_contents($file);
            Assert::eq($content, $data);

            // Cleanup
            unlink($file);

            $wg->done();
        });
    }

    $wg->wait();
    echo "all {$n} coroutines completed\n";
});

echo "DONE\n";
?>
--EXPECT--
all 10 coroutines completed
DONE
