--TEST--
openswoole_function: OpenSwoole\Util::getCPUNum
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$cpu_num = OpenSwoole\Util::getCPUNum();
echo "cpu_num: $cpu_num";

?>
--EXPECTF--
cpu_num: %d
