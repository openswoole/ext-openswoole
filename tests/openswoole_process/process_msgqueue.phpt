--TEST--
openswoole_process: sysv msgqueue
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function callback_function(openswoole_process $worker){}

$process = new openswoole_process('callback_function', false, 0);
$process->useQueue();

$bytes = 0;
foreach(range(1, 10) as $i)
{
    $data = "hello worker[$i]";
    $bytes += strlen($data);
    $process->push($data);
}

$queue = $process->statQueue();
($queue['queue_num'] == 10 && $queue['queue_bytes'] == $bytes)
    && $output = "Success\n";

echo $output;
$process->freeQueue();
?>
Done
--EXPECTREGEX--
Success
Done.*
