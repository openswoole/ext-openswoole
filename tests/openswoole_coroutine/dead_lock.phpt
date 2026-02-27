--TEST--
openswoole_coroutine: dead lock
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Process;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    for ($n = 3; $n--;) {
        $ret = Process::wait(false);
        Assert::isEmpty($ret);
        switch_process();
    }
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $pm->wakeup();
    Coroutine::set([
        'exit_condition' => function () {
            return Coroutine::stats()['coroutine_num'] === 0;
        }
    ]);
    Co::run(function () {
        $channel = new Coroutine\Channel;
        $channel->pop();
    });
};
$pm->childFirst();
$pm->run();

?>
--EXPECT--
DONE
