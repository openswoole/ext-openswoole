--TEST--
swoole_mysql_coro: reset test mysql database
--CONFLICTS--
swoole_mysql_coro
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$init_script = __DIR__ . '/../init';
`php {$init_script} > /dev/null`;
?>
--EXPECT--
