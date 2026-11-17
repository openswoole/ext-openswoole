--TEST--
openswoole_runtime: hooked time_nanosleep() rejects nanoseconds out of range like the native function
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
co::run(function () {
    try {
        time_nanosleep(0, 1500000000);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump(time_nanosleep(0, 1000));
});
?>
--EXPECT--
Nanoseconds was not in the range 0 to 999 999 999 or seconds was negative
bool(true)
