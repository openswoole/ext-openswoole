<?php

// $chan1 = new Swoole\Coroutine\Channel(3);
// $chan2 = new Swoole\Coroutine\Channel(3);

// Co\run(function () use ($chan1) {
//     $cid = Swoole\Coroutine::getuid();
//     $i = 0;
//     while (1) {
//         co::sleep(1.0);
//         $chan1->push(['rand' => rand(1000, 9999), 'index' => $i]);
//         echo "[coroutine $cid] - $i\n";
//         $i++;
//     }
// });
// Co\run(function () use ($chan2) {
//     $cid = Swoole\Coroutine::getuid();
//     $i = 0;
//     while (1) {
//         co::sleep(1.0);
//         $chan2->push(['rand' => rand(1000, 9999), 'index' => $i]);
//         echo "[coroutine $cid] - $i\n";
//         $i++;
//     }
// });
// Co\run(function () use ($chan1, $chan2) {
//     $cid = Swoole\Coroutine::getuid();
//     while(1) {

//      $data = $chan1->pop();
//      echo "[coroutine $cid]\n";
//         var_dump($data);

//         // pop, push, timeout
//      $chan = Swoole\Coroutine::select([$chan1, $chan2], [$chan1, $chan2], 1);
//         echo "[coroutine $cid]\n";
//         $data = $chan->pop();
//         var_dump($data);

//         $data = $chan2->pop();
//      echo "[coroutine $cid]\n";
//         var_dump($data);
//     }
// });

Co\run(function () {
    $chan1 = new Swoole\Coroutine\Channel(1);
    $chan2 = new Swoole\Coroutine\Channel(1);

    go(function() use ($chan1) {
        $wait = random_int(1,9);
        echo "chan1 wait: $wait\n";
        co::sleep($wait);
        $chan1->push(['rand' => $wait, 'index' => 'chan1']);
    });

    go(function() use ($chan2) {
        $wait = random_int(1,9);
        echo "chan2 wait: $wait\n";
        co::sleep($wait);
        $chan2->push(['rand' => $wait, 'index' => 'chan2']);
    });
    var_dump([$chan1, $chan2]);
    $cid = Swoole\Coroutine::getuid();
    $i = 0;

    // $chan1->push(['rand' => rand(1000, 9999), 'index' => $i++]);
    // $chan2->push(['rand' => rand(1000, 9999), 'index' => $i++]);

    $start = time();
    $chan = Swoole\Coroutine::select([$chan1, $chan2], [], 10);
    $cost = time() - $start;
    echo "SELECT DONE: $cost\n";
});