--TEST--
openswoole_io_uring: phpinfo shows io_uring enabled
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_io_uring();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();

Assert::assert(strpos($info, 'io_uring') !== false);
Assert::assert(strpos($info, 'io_uring => enabled') !== false);
echo "DONE\n";
?>
--EXPECT--
DONE
