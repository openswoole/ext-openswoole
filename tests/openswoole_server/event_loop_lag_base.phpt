--TEST--
openswoole_server: event_loop_lag in stats (BASE mode)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;
use OpenSwoole\Client;

$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {
    // wait for the lag timer to fire at least once (1s interval + margin)
    usleep(1500 * 1000);

    $client = new Client(OPENSWOOLE_SOCK_TCP, OPENSWOOLE_SOCK_SYNC);
    if (!$client->connect('127.0.0.1', $pm->getFreePort())) {
        exit("connect failed\n");
    }
    $client->send("hello");
    $data = $client->recv();
    Assert::assert($data);
    $stats = json_decode($data, true);

    // top-level lag fields
    Assert::assert(array_key_exists('event_loop_lag_ms', $stats));
    Assert::assert(array_key_exists('event_loop_lag_max_ms', $stats));
    Assert::assert(array_key_exists('event_loop_lag_avg_ms', $stats));

    Assert::assert(is_float($stats['event_loop_lag_ms']) || is_int($stats['event_loop_lag_ms']));
    Assert::assert($stats['event_loop_lag_ms'] >= 0);
    Assert::assert($stats['event_loop_lag_max_ms'] >= 0);
    Assert::assert($stats['event_loop_lag_avg_ms'] >= 0);

    // per event-worker lag fields
    Assert::assert(isset($stats['event_workers']));
    foreach ($stats['event_workers'] as $worker) {
        Assert::assert(array_key_exists('event_loop_lag_ms', $worker));
        Assert::assert(array_key_exists('event_loop_lag_max_ms', $worker));
        Assert::assert(array_key_exists('event_loop_lag_avg_ms', $worker));
        Assert::assert($worker['event_loop_lag_ms'] >= 0);
        Assert::assert($worker['event_loop_lag_max_ms'] >= 0);
        Assert::assert($worker['event_loop_lag_avg_ms'] >= 0);
    }

    echo "DONE\n";
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), OPENSWOOLE_BASE, OPENSWOOLE_SOCK_TCP);
    $serv->set([
        'worker_num' => 2,
        'log_level' => OPENSWOOLE_LOG_ERROR,
        'enable_coroutine' => false,
    ]);
    $counter = new OpenSwoole\Atomic(0);
    $serv->on('workerStart', function (Server $serv, $wid) use ($pm, $counter) {
        if ($counter->add(1) == $serv->setting['worker_num']) {
            $pm->wakeup();
        }
    });
    $serv->on('receive', function (Server $serv, $fd, $tid, $data) {
        $serv->send($fd, json_encode($serv->stats()));
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
