--TEST--
openswoole_coroutine_system: a second waitSignal() for the same signal fails with EBUSY
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\System;

co::run(function () {
    go(function () {
        var_dump(System::waitSignal(SIGUSR1, 3));
    });
    go(function () {
        var_dump(@System::waitSignal(SIGUSR1, 3));
        var_dump(OpenSwoole\Util::getLastErrorCode() === SOCKET_EBUSY);
    });
    go(function () {
        co::usleep(100000);
        OpenSwoole\Process::kill(getmypid(), SIGUSR1);
    });
});
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
