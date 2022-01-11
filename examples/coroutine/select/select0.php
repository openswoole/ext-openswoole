<?php declare(strict_types = 1);

Co\run(function () {
    $chan1 = new Swoole\Coroutine\Channel(1);
    $chan2 = new Swoole\Coroutine\Channel(1);

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
    $cid = Swoole\Coroutine::getuid();
    $i = 0;

    // $chan1->push(['rand' => rand(1000, 9999), 'index' => $i++]);
    // $chan2->push(['rand' => rand(1000, 9999), 'index' => $i++]);

    $start = time();
    $ret = Swoole\Coroutine::select([$chan1, $chan2], [], 10);
    echo "select result:".var_export($ret, true)."\n";
    $cost = time() - $start;
    echo "SELECT DONE after $cost seconds\n";
    echo "waiting for shutdown..\n";
});