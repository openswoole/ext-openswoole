--TEST--
swoole_io_uring: concurrent coroutine file I/O with io_uring engine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    $n = 5;
    $chan = new OpenSwoole\Coroutine\Channel($n);

    for ($i = 0; $i < $n; $i++) {
        go(function () use ($i, $chan) {
            $tmpfile = tempnam(sys_get_temp_dir(), "swoole_iouring_{$i}_");
            $data = "coroutine $i payload " . str_repeat('x', 1024);

            file_put_contents($tmpfile, $data);
            $content = file_get_contents($tmpfile);
            unlink($tmpfile);

            if ($content === $data && !file_exists($tmpfile)) {
                $chan->push("coroutine $i ok");
            } else {
                $chan->push("coroutine $i FAIL");
            }
        });
    }

    $results = [];
    for ($i = 0; $i < $n; $i++) {
        $results[] = $chan->pop();
    }
    sort($results);
    foreach ($results as $r) {
        echo "$r\n";
    }
});

echo "DONE\n";
?>
--EXPECT--
coroutine 0 ok
coroutine 1 ok
coroutine 2 ok
coroutine 3 ok
coroutine 4 ok
DONE
