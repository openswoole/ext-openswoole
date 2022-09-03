--TEST--
swoole_function: set process name
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_darwin();
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$name = "SWOOLE_PROCESS_TEST_" . rand(1, 100);
OpenSwoole\Util::setProcessName($name);
$count = (int)trim(`ps aux|grep $name|grep -v grep|wc -l`);
Assert::same($count, 1);
echo "SUCCESS";

?>
--EXPECT--
SUCCESS
