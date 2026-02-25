--TEST--
swoole_io_uring: file I/O operations via io_uring engine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);
$test_file = '/tmp/swoole_io_uring_phpt_test_' . getmypid();
$test_file2 = $test_file . '_renamed';
$test_dir = $test_file . '_dir';

Co\run(function () use ($test_file, $test_file2, $test_dir) {
    // Test file_put_contents (uses open + write)
    $data = "hello io_uring file I/O";
    $n = file_put_contents($test_file, $data);
    Assert::eq($n, strlen($data));
    echo "write: OK\n";

    // Test file_get_contents (uses open + read)
    $content = file_get_contents($test_file);
    Assert::eq($content, $data);
    echo "read: OK\n";

    // Test filesize (uses stat)
    clearstatcache();
    $size = filesize($test_file);
    Assert::eq($size, strlen($data));
    echo "stat: OK\n";

    // Test rename
    rename($test_file, $test_file2);
    Assert::true(file_exists($test_file2));
    Assert::false(file_exists($test_file));
    echo "rename: OK\n";

    // Test unlink
    unlink($test_file2);
    Assert::false(file_exists($test_file2));
    echo "unlink: OK\n";

    // Test mkdir
    mkdir($test_dir, 0755);
    Assert::true(is_dir($test_dir));
    echo "mkdir: OK\n";

    // Test rmdir
    rmdir($test_dir);
    Assert::false(is_dir($test_dir));
    echo "rmdir: OK\n";
});

echo "DONE\n";
?>
--EXPECT--
write: OK
read: OK
stat: OK
rename: OK
unlink: OK
mkdir: OK
rmdir: OK
DONE
