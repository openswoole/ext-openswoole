--TEST--
openswoole_process: push
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

//$proc = new \openswoole_process(openswoole_function() {});
//$proc->useQueue();
//$r = $proc->push("\0");
// Assert::false($r);
// TODO max data ?
// $r = $proc->push(str_repeat("\0", 1024 * 1024 * 8));
// Assert::false($r);
//$proc->freeQueue();

$proc = new \openswoole_process(function() {});
$proc->useQueue();
$proc->start();
$r = $proc->push("\0");
Assert::true($r);
$proc->freeQueue();
\openswoole_process::wait(true);
echo "SUCCESS";
?>
--EXPECT--
SUCCESS
