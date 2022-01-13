--TEST--
swoole_coroutine/async_callback: signal
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use Swoole\Process;

Co\run(function () {
    Process::signal(SIGUSR1, function ($signo) {
        co::usleep(500000);
        var_dump($signo);
    });

    co::usleep(10000);
    Process::kill(posix_getpid(), SIGUSR1);
    co::usleep(20000);
});
?>
--EXPECT--
int(10)
