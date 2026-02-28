--TEST--
openswoole_coroutine: dead lock
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; skip("TODOv22") ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Process;

ini_set('openswoole.enable_coroutine', 'off');

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    for ($n = 3; $n--;) {
        $ret = Process::wait(false);
        Assert::isEmpty($ret);
        switch_process();
    }
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $pm->wakeup();
    Coroutine::set([
        'exit_condition' => function () {
            return Coroutine::stats()['signal_listener_num'] === 0;
        }
    ]);
    // Process::signal(SIGINT, function () {
    //     echo 'SIGINT' . PHP_EOL;
    //     exit(123);
    // });
    Process::signal(SIGTERM, function () {
        echo 'SIGTERM' . PHP_EOL;
        exit(123);
    });
};
$pm->childFirst();
$pm->run();
$pm->expectExitCode(123);

?>
--EXPECT--
SIGTERM
