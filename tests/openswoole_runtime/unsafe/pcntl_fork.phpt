--TEST--
openswoole_runtime/unsafe: pcntl_fork
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
skip_if_offline();
skip_if_function_not_exist('pcntl_fork');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';



co::run(function ()  {
    pcntl_fork();
});
?>
--EXPECTF--
Warning: pcntl_fork() has been disabled for security reasons in %s on line %d
