--TEST--
swoole_io_uring: error paths for file operations with io_uring engine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    $nonexistent = '/tmp/swoole_iouring_nonexistent_' . getmypid() . '_' . mt_rand();

    // file_get_contents on nonexistent file should return false
    $result = @file_get_contents($nonexistent);
    Assert::false($result);

    // stat on nonexistent file
    $result = @stat($nonexistent);
    Assert::false($result);

    // unlink on nonexistent file should return false
    $result = @unlink($nonexistent);
    Assert::false($result);

    // rmdir on nonexistent directory should return false
    $result = @rmdir($nonexistent);
    Assert::false($result);

    echo "DONE\n";
});
?>
--EXPECT--
DONE
