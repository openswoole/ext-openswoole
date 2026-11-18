--TEST--
openswoole_process: wait_signal keeps waiting while a handler is registered
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Process;
use OpenSwoole\Event;

$proc = new Process(function (Process $process) {
    OpenSwoole\Util::setAio(['wait_signal' => true]);
    $count = 0;
    Process::signal(SIGUSR1, function () use (&$count, $process) {
        $count++;
        $process->write("SIGUSR1 {$count}\n");
        if ($count == 2) {
            Process::signal(SIGUSR1, null);
        }
    });
    $process->write("START\n");
    Event::wait();
    $process->write("EXIT\n");
}, false, SOCK_STREAM);

$pid = $proc->start();
echo $proc->read();
Process::kill($pid, SIGUSR1);
echo $proc->read();
Process::kill($pid, SIGUSR1);
echo $proc->read();
echo $proc->read();
$retval = Process::wait(true);
Assert::eq($retval['pid'], $pid);
Assert::eq($retval['signal'], 0);
?>
--EXPECT--
START
SIGUSR1 1
SIGUSR1 2
EXIT
