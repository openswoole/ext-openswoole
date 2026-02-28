--TEST--
openswoole_process: push
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Process;

$process = new Process(function(Process $worker) {

  $recv = $worker->pop();

  echo "$recv";
  usleep(20000);

  $worker->exit(0);
}, false, 0);

$process->useQueue();
$pid = $process->start();

$process->push("hello worker\n");
Process::wait();
?>
--EXPECT--
hello worker
