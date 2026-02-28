<?php
use OpenSwoole\Server;
use OpenSwoole\Timer;

$server = new Server('0.0.0.0', 9501, OPENSWOOLE_BASE);
$server->set([
    'worker_num' => 1,
    'enable_coroutine' => false, // disable so sleep() truly blocks the event loop
]);

$server->on('workerStart', function (Server $server) {
    // Print lag stats every 3 seconds
    Timer::tick(3000, function () use ($server) {
        $stats = $server->stats();
        echo sprintf(
            "lag: %.2fms | max: %.2fms | avg: %.2fms\n",
            $stats['event_loop_lag_ms'],
            $stats['event_loop_lag_max_ms'],
            $stats['event_loop_lag_avg_ms']
        );
    });
});

$server->on('receive', function (Server $server, $fd, $reactor_id, $data) {
    // Simulate blocking work — this will spike the lag
    usleep(500 * 1000); // 500ms block
    $server->send($fd, "OK\n");
});

$server->start();