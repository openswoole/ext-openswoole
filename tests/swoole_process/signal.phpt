--TEST--
swoole_process: signal
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Process;

//父进程中先设置信号
Process::signal(SIGCHLD, function ()
{
    Process::signal(SIGCHLD, null);
    Process::signal(SIGTERM, null);
    OpenSwoole\Event::del(STDIN);
    OpenSwoole\Timer::clearAll();
    echo "PARENT WAIT\n";
});

//测试被子进程覆盖信号
Process::signal(SIGTERM, function () {
    //释放信号，否则底层会报内存泄漏
    Process::signal(SIGTERM, null);
    echo "PARENT SIGTERM\n";
    OpenSwoole\Event::exit();
});

$pid = (new Process(function ()
{
    Process::signal(SIGTERM, function ($sig) {
        echo "CHILD SIGTERM\n";
        Process::signal(SIGTERM, function ($sig) {
            echo "CHILD EXIT\n";
            OpenSwoole\Event::del(STDIN);
        });
    });

    //never calback
    OpenSwoole\Event::add(STDIN, function () {});

    OpenSwoole\Event::wait();


}))->start();

OpenSwoole\Timer::after(1000, function() use ($pid) {
    Process::kill($pid, SIGTERM);
    OpenSwoole\Timer::after(1000, function() use ($pid) {
        Process::kill($pid, SIGTERM);
    });
});

//never calback
OpenSwoole\Event::add(STDIN, function ($fp) {
    echo fread($fp, 8192);
});

OpenSwoole\Event::wait();
?>
--EXPECT--
CHILD SIGTERM
CHILD EXIT
PARENT WAIT
