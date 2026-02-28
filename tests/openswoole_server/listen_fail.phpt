--TEST--
openswoole_server: listen fail
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
//调高log_level
Co::set(['log_level' => SWOOLE_LOG_NONE]);
try {
    $serv = new openswoole_server('192.0.0.1', 80);
} catch (openswoole_exception $e) {
    Assert::same($e->getCode(), SOCKET_EADDRNOTAVAIL);
    echo "DONE\n";
}
?>
--EXPECT--
DONE
