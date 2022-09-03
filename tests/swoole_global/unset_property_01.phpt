--TEST--
swoole_global: unset internal class property
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$c = new chan;
unset($c->capacity);
?>
--EXPECTF--
Fatal error: Uncaught Error: Property capacity of class OpenSwoole\Coroutine\Channel cannot be unset in %s/tests/%s/unset%s.php:%d
Stack trace:
#0 {main}
  thrown in %s
