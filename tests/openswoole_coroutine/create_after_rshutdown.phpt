--TEST--
openswoole_coroutine: create coroutine after RSHTUDOWN
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
register_shutdown_function(function () {
    go(function () {
        co::usleep(10000);
        echo "DONE\n";
    });
    openswoole_event::wait();
});
exit(0);

?>
--EXPECTF--
DONE
