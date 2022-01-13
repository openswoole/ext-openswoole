--TEST--
swoole_runtime: stdin
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);

use Swoole\Process;
use Swoole\Runtime;

require __DIR__ . '/../include/bootstrap.php';

Co::set(['socket_read_timeout' => -1, ]);

$proc = new Process(function ($p) {
    Co\run(function () use($p) {
        $p->write('start'.PHP_EOL);
        go(function() {
            co::usleep(50000);
            echo "sleep\n";
        });
        echo fread(STDIN, 1024);
    });
}, true, SOCK_STREAM);
$proc->start();

echo $proc->read();

usleep(100000);
$proc->write('hello world'.PHP_EOL);

echo $proc->read();
echo $proc->read();

Process::wait()
?>
--EXPECT--
start
sleep
hello world
