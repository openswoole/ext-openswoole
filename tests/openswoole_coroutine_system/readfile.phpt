--TEST--
openswoole_coroutine_system: readFile
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

go(function () {
    $content = OpenSwoole\Coroutine\System::readFile(TEST_IMAGE);
    Assert::same(md5_file(TEST_IMAGE), md5($content));
});
?>
--EXPECT--
