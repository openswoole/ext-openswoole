--TEST--
openswoole_coroutine/async_callback: event cycle
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Event;

const N = 4;

$GLOBALS['count'] = 0;
$GLOBALS['logs'] = [];

co::run(function () {
    Event::cycle(function () {
        $GLOBALS['count']++;
        if ($GLOBALS['count'] == N) {
            Event::cycle(null);
        }
        co::usleep(20000);
        $GLOBALS['logs'] [] = "cycle\n";
    });

    $n = N;
    while ($n--) {
        co::usleep(10000);
        $GLOBALS['logs'] [] = "sleep\n";
    }
});

$str = implode('', $GLOBALS['logs']);
Assert::eq(substr_count($str, 'cycle'), N);
Assert::eq(substr_count($str, 'sleep'), N);
?>
--EXPECT--
