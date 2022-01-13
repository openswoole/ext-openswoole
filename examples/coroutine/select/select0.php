<?php declare(strict_types = 1);

// Example of co::select

Co\run(function () {
    $chan1 = new chan(1);
    $chan2 = new chan(1);

    go(function() use ($chan1) {
        $wait = 4;
        echo "chan1 wait: $wait seconds\n";
        co::sleep($wait);
        $chan1->push(['rand' => $wait, 'index' => 'chan1']);
    });

    go(function() use ($chan2) {
        $wait = 2;
        echo "chan2 wait: $wait seconds\n";
        co::sleep($wait);
        $chan2->push(['rand' => $wait, 'index' => 'chan2']);
    });
    var_dump([$chan1, $chan2]);
    $i = 0;

    // $chan1->push(['rand' => rand(1000, 9999), 'index' => $i++]);
    // $chan2->push(['rand' => rand(1000, 9999), 'index' => $i++]);

    $start = time();
    $ret = co::select([$chan1, $chan2], [], 10);
    echo "select result:".var_export($ret, true)."\n";
    $cost = time() - $start;
    echo "SELECT DONE after $cost seconds\n";
    echo "waiting for shutdown..\n";
});