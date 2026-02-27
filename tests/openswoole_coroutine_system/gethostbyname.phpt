--TEST--
openswoole_coroutine_system: gethostbyname
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Coroutine::create(function () {
    $ip = OpenSwoole\Coroutine\System::gethostbyname('www.baidu.com');
    Assert::assert($ip != false);
});

?>
--EXPECT--
