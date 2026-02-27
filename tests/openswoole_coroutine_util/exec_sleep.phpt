--TEST--
openswoole_coroutine_util: coroutine exec
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$s = microtime(true);
for ($i = MAX_PROCESS_NUM; $i--;) {
    go(function () {
        co::exec('sleep 1');
    });
}
openswoole_event_wait();
$s = microtime(true) - $s;
time_approximate(1, $s);
echo "DONE\n";
?>
--EXPECT--
DONE
