--TEST--
openswoole_atomic: destruct objects in child processe
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$atomic = new openswoole_atomic;

$p = new openswoole_process(function () use ($atomic) {
    $atomic->wait();
    echo "Child OK\n";
    exit(0);
});
$p->start();

usleep(200000);
echo "Master OK\n";
$atomic->wakeup(1);
$status = openswoole_process::wait();
?>
--EXPECT--
Master OK
Child OK
