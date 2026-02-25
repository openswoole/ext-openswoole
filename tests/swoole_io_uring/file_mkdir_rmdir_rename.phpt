--TEST--
swoole_io_uring: mkdir, rename, rmdir with io_uring engine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    $base = sys_get_temp_dir() . '/swoole_iouring_test_' . getmypid();

    // mkdir
    $dir = $base . '_dir';
    Assert::true(mkdir($dir));
    Assert::true(is_dir($dir));

    // Create a file inside the directory
    $file = $dir . '/testfile.txt';
    file_put_contents($file, 'test');
    Assert::same(file_get_contents($file), 'test');

    // rename the file
    $renamed = $dir . '/renamed.txt';
    Assert::true(rename($file, $renamed));
    Assert::false(file_exists($file));
    Assert::same(file_get_contents($renamed), 'test');

    // Clean up
    unlink($renamed);

    // rmdir
    Assert::true(rmdir($dir));
    Assert::false(is_dir($dir));

    echo "DONE\n";
});
?>
--EXPECT--
DONE
