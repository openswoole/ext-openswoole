--TEST--
openswoole_process: read
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$proc = new \openswoole_process(function(\openswoole_process $process) {
    $r = $process->write("SUCCESS");
    Assert::same($r, 7);
});
$r = $proc->start();
Assert::assert($r > 0);

openswoole_timer_after(10, function() use($proc) {
    echo $proc->read();
    // openswoole_event_exit();
});

OpenSwoole\Event::wait(true);
?>
--EXPECT--
SUCCESS
