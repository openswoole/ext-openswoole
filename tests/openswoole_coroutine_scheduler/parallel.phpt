--TEST--
openswoole_coroutine_scheduler: user yield and resume1
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$sch = new OpenSwoole\Coroutine\Scheduler;

$sch->set(['max_coroutine' => 100]);

$sch->parallel(10, function ($t, $n) {
    Co::usleep($t);
    echo "Co ".Co::getCid()."\n";
}, 50000, 'A');

$sch->start();

?>
--EXPECTF--
Co %d
Co %d
Co %d
Co %d
Co %d
Co %d
Co %d
Co %d
Co %d
Co %d
