--TEST--
openswoole_coroutine_system: gethostbyname
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Coroutine::create(function () {
    $ip = OpenSwoole\Coroutine\System::gethostbyname('localhost');
    Assert::same($ip, '127.0.0.1');
});

?>
--EXPECT--
