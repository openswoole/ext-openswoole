--TEST--
openswoole_process: read() rejects a buffer size of 0 or less
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$process = new OpenSwoole\Process(function (OpenSwoole\Process $process) {
    $process->write(str_repeat('A', 4096));
}, false, SOCK_STREAM);
$process->start();
usleep(100000);
var_dump($process->read(-1));
var_dump($process->read(0));
var_dump(strlen($process->read(10)));
OpenSwoole\Process::wait();
?>
--EXPECTF--
Warning: OpenSwoole\Process::read(): the buffer size must be greater than 0 in %s on line %d
bool(false)

Warning: OpenSwoole\Process::read(): the buffer size must be greater than 0 in %s on line %d
bool(false)
int(10)
