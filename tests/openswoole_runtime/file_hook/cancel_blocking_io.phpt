--TEST--
openswoole_runtime/file_hook: a coroutine blocked in the thread pool can not be canceled
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Coroutine\System;

$file = sys_get_temp_dir() . '/openswoole_cancel_blocking_io';
@unlink($file);
file_put_contents($file, '');

$holder = new OpenSwoole\Process(function (OpenSwoole\Process $process) use ($file) {
    $fp = fopen($file, 'r+');
    flock($fp, LOCK_EX);
    $process->write('locked');
    usleep(500000);
    flock($fp, LOCK_UN);
}, false, SOCK_STREAM);
$holder->start();
$holder->read();

co::run(function () use ($file) {
    // open(), flock() and write() run in a pool thread that writes to this coroutine's stack
    $writer = go(function () use ($file) {
        var_dump(System::writeFile($file, 'hello', LOCK_EX));
    });
    co::usleep(100000);
    var_dump(OpenSwoole\Coroutine::cancel($writer));
});
OpenSwoole\Process::wait();
var_dump(file_get_contents($file));
@unlink($file);
?>
--EXPECT--
bool(false)
int(5)
string(5) "hello"
