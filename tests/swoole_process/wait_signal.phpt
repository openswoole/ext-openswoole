--TEST--
swoole_process: wait signal
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Process;
use Swoole\Event;

$proc = new Process(function(Process $process) {
    OpenSwoole\Util::setAio(['wait_signal' => true]);
    Process::signal(SIGINT, function () {
        echo "SIGINT\n";
        // Process::signal(SIGINT, null);
    });
    echo "START\n";
    Event::wait();
}, true, 1);

$r = $proc->start();
Assert::assert($r > 0);

echo $proc->read();
Process::kill((int)$r, SIGINT);
echo $proc->read();

$retval = Process::wait(true);
Assert::eq($retval['pid'], $r);
Assert::eq($retval['code'], 0);
Assert::eq($retval['signal'], 0);
?>
--EXPECT--
START
SIGINT
