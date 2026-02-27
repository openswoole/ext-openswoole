--TEST--
openswoole_process: close
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

//$proc = new \openswoole_process(openswoole_function(\openswoole_process $proc) {
//    $proc->close();
//});
//$proc->start();
//
//$proc = new \openswoole_process(openswoole_function(\openswoole_process $proc) {
//    usleep(200000);
//    // Assert::true(false, 'never here');
//});
//$proc->start();
//$proc->close();
//
//
//\openswoole_process::wait(true);
//\openswoole_process::wait(true);
echo "SUCCESS";
?>
--EXPECT--
SUCCESS
