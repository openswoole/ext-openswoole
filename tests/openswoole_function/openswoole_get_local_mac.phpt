--TEST--
openswoole_function: get mac address
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$macs = OpenSwoole\Util::getLocalMac();
Assert::assert(is_array($macs));
foreach ($macs as $mac) {
    Assert::same(filter_var($mac, FILTER_VALIDATE_MAC), $mac);
}

?>
--EXPECT--
