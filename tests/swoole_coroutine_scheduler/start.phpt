--TEST--
swoole_coroutine_scheduler: user yield and resume1
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$sch = new OpenSwoole\Coroutine\Scheduler;

$sch->set(['max_coroutine' => 100]);

$sch->add(function ($t, $n) {
    Co::usleep($t);
    echo "$n\n";
}, 200000, 'A');

$sch->add(function ($t, $n) {
    Co::usleep($t);
    echo "$n\n";
}, 100000, 'B');

$sch->add(function () {
    var_dump(Co::getCid());
});

$sch->start();

?>
--EXPECTF--
int(%d)
B
A
