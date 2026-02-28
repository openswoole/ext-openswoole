--TEST--
openswoole_server: event_loop_lag increases under blocking load
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;
use OpenSwoole\Coroutine\Client;

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {
    // wait for lag timer baseline (>1s)
    usleep(1500 * 1000);

    co::run(function () use ($pm) {
        $client = new Client(OPENSWOOLE_SOCK_TCP);
        $client->set(['timeout' => 10]);
        if (!$client->connect('127.0.0.1', $pm->getFreePort(), 10)) {
            exit("connect failed\n");
        }

        // First request: trigger blocking sleep(1) on the server
        $client->send("block");
        $data = $client->recv();
        Assert::assert($data === "ok");

        // Wait briefly so the lag timer fires after the blocking sleep
        co::usleep(100);

        // Second request: read stats after the lag timer has measured the delay
        $client->send("stats");
        $data = $client->recv();
        Assert::assert($data);
        $stats = json_decode($data, true);

        // The blocking sleep should have caused measurable lag captured by lag_max_ms
        Assert::assert($stats['event_loop_lag_max_ms'] >= 200,
            "expected max lag >= 200ms, got " . $stats['event_loop_lag_max_ms']);
        Assert::assert($stats['event_loop_lag_avg_ms'] > 0);

        echo "DONE\n";
        $pm->kill();
    });
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), OPENSWOOLE_BASE, OPENSWOOLE_SOCK_TCP);
    $serv->set([
        'worker_num' => 1,
        'log_level' => OPENSWOOLE_LOG_ERROR,
        'enable_coroutine' => false,
    ]);
    $serv->on('workerStart', function (Server $serv, $wid) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function (Server $serv, $fd, $tid, $data) {
        if ($data === "block") {
            // block the event loop for 1 second
            sleep(1);
            $serv->send($fd, "ok");
        } elseif ($data === "stats") {
            $serv->send($fd, json_encode($serv->stats()));
        }
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
