--TEST--
openswoole_function: OpenSwoole\Util::getVersion
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$version = OpenSwoole\Util::getVersion();
echo "OpenSwoole: $version";

?>
--EXPECTF--
OpenSwoole: %s
