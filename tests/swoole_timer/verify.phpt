--TEST--
swoole_timer: verify timer
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
for ($c = MAX_CONCURRENCY; $c--;) {
    go(function () {
        $seconds = ms_random(0.1, 0.5);
        $start = microtime(true);
        Co::usleep(intval($seconds * 1000000));
        time_approximate($seconds, microtime(true) - $start, 0.25);
    });
}
Swoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
DONE
