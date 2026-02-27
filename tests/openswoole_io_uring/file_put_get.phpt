--TEST--
openswoole_io_uring: file_put_contents and file_get_contents with io_uring engine
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Co::set(['reactor_type' => OPENSWOOLE_IO_URING]);

Co::run(function () {
    $tmpfile = tempnam(sys_get_temp_dir(), 'openswoole_iouring_');
    file_put_contents($tmpfile, "hello io_uring");
    $content = file_get_contents($tmpfile);
    Assert::same($content, "hello io_uring");
    unlink($tmpfile);
    Assert::false(file_exists($tmpfile));
    echo "DONE\n";
});
?>
--EXPECT--
DONE
