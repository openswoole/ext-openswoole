--TEST--
openswoole_process/coro: ipc with coroutine
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$proc1 = new \openswoole_process(function (openswoole_process $proc) {
    $socket = $proc->exportSocket();
    echo $socket->recv();
    $socket->send("hello proc2\n");
    echo "proc1 stop\n";
}, false, 1, true);

Assert::assert($proc1->start());

$proc2 = new \openswoole_process(function (openswoole_process $proc) use ($proc1) {
    co::usleep(10000);
    $socket = $proc1->exportSocket();
    $socket->send("hello proc1\n");
    echo $socket->recv();
    echo "proc2 stop\n";
}, false, 0, true);

Assert::assert($proc2->start());

openswoole_process::wait(true);
openswoole_process::wait(true);

?>
--EXPECT--
hello proc1
proc1 stop
hello proc2
proc2 stop
