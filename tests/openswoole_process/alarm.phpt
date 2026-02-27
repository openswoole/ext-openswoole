--TEST--
openswoole_process: alarm
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Process;

Process::signal(SIGALRM, function () {
    static $i = 0;
    echo "#{$i}\talarm\n";
    $i++;
    if ($i > 10) {
        Process::alarm(-1);
        Process::signal(SIGALRM, null);
        OpenSwoole\Event::del(STDIN);
        openswoole_event_exit();
    }
});

//100ms
Process::alarm(10 * 1000);

//never calback
OpenSwoole\Event::add(STDIN, function () {});

OpenSwoole\Event::wait();

?>
--EXPECT--
#0	alarm
#1	alarm
#2	alarm
#3	alarm
#4	alarm
#5	alarm
#6	alarm
#7	alarm
#8	alarm
#9	alarm
#10	alarm
