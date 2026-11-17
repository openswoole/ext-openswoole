--TEST--
openswoole_process: freeing a process closes its pipe
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_file_not_exist('/proc/self/fd');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$count = function () {
    return count(scandir('/proc/self/fd'));
};
$before = $count();
for ($i = 0; $i < 200; $i++) {
    $process = new OpenSwoole\Process(function () {}, false, SOCK_DGRAM);
    unset($process);
}
var_dump($count() - $before);
?>
--EXPECT--
int(0)
