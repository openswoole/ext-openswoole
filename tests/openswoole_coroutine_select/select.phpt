--TEST--
openswoole_coroutine_select: select
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
Co::run(function() {
    $chan1 = new OpenSwoole\Coroutine\Channel(1);
    $chan2 = new OpenSwoole\Coroutine\Channel(1);
    $wait_min = 10;

    go(function() use ($chan1, &$wait_min) {
        $wait = random_int(1, 9);
        if($wait < $wait_min) {
            $wait_min = $wait;
        }
        echo "chan1 wait: $wait\n";
        co::sleep($wait);
        $chan1->push(['rand' => $wait, 'index' => 'chan1']);
    });

    go(function() use ($chan2, &$wait_min) {
        $wait = random_int(1, 9);
        if($wait < $wait_min) {
            $wait_min = $wait;
        }
        echo "chan2 wait: $wait\n";
        co::sleep($wait);
        $chan2->push(['rand' => $wait, 'index' => 'chan2']);
    });
    $i = 0;
    $start = time();
    $chan = OpenSwoole\Coroutine::select([$chan1, $chan2], [], 10);
    $cost = time() - $start;
    Assert::same($wait_min, $cost);
    echo "SELECT DONE: $wait_min\n";
});
?>
--EXPECTF--
chan1 wait: %d
chan2 wait: %d
SELECT DONE: %d
