--TEST--
swoole_coroutine_wait_group: empty
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; skip('TODOv22'); ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$wg = new OpenSwoole\Coroutine\WaitGroup;
$wg->add(1);
$wg->add(-1);
$wg->wait();
$wg->add(1);
$wg->done();
$wg->wait();
echo "DONE\n";
?>
--EXPECT--
DONE
