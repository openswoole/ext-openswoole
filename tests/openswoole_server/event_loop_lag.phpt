--TEST--
openswoole_server: event_loop_lag in stats (PROCESS mode)
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
    // wait for the lag timer to fire at least once (1s interval + margin)
    usleep(1500 * 1000);

    makeCoTcpClient('127.0.0.1', $pm->getFreePort(), function (Client $cli) {
        $r = $cli->send(opcode_encode("stats", []));
        Assert::assert($r !== false);
    }, function (Client $cli, $recv) use ($pm) {
        list($op, $data) = opcode_decode($recv);
        $cli->close();

        // top-level lag fields
        Assert::assert(array_key_exists('event_loop_lag_ms', $data));
        Assert::assert(array_key_exists('event_loop_lag_max_ms', $data));
        Assert::assert(array_key_exists('event_loop_lag_avg_ms', $data));

        Assert::assert(is_float($data['event_loop_lag_ms']) || is_int($data['event_loop_lag_ms']));
        Assert::assert($data['event_loop_lag_ms'] >= 0);
        Assert::assert($data['event_loop_lag_max_ms'] >= 0);
        Assert::assert($data['event_loop_lag_avg_ms'] >= 0);
        Assert::assert($data['event_loop_lag_max_ms'] >= $data['event_loop_lag_avg_ms']);

        // per event-worker lag fields
        Assert::assert(isset($data['event_workers']));
        foreach ($data['event_workers'] as $worker) {
            Assert::assert(array_key_exists('event_loop_lag_ms', $worker));
            Assert::assert(array_key_exists('event_loop_lag_max_ms', $worker));
            Assert::assert(array_key_exists('event_loop_lag_avg_ms', $worker));
            Assert::assert($worker['event_loop_lag_ms'] >= 0);
            Assert::assert($worker['event_loop_lag_max_ms'] >= 0);
            Assert::assert($worker['event_loop_lag_avg_ms'] >= 0);
        }

        // reactor thread lag fields (PROCESS mode only)
        Assert::assert(isset($data['reactor_threads']));
        Assert::assert(count($data['reactor_threads']) > 0);
        foreach ($data['reactor_threads'] as $rt) {
            Assert::assert(array_key_exists('reactor_id', $rt));
            Assert::assert(array_key_exists('event_loop_lag_ms', $rt));
            Assert::assert(array_key_exists('event_loop_lag_max_ms', $rt));
            Assert::assert(array_key_exists('event_loop_lag_avg_ms', $rt));
            Assert::assert($rt['event_loop_lag_ms'] >= 0);
            Assert::assert($rt['event_loop_lag_max_ms'] >= 0);
            Assert::assert($rt['event_loop_lag_avg_ms'] >= 0);
        }

        echo "DONE\n";
        $pm->kill();
    });
    OpenSwoole\Event::wait();
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), OPENSWOOLE_PROCESS, OPENSWOOLE_SOCK_TCP);
    $serv->set([
        'worker_num' => 2,
        'task_worker_num' => 1,
        'log_level' => OPENSWOOLE_LOG_ERROR,
        'open_length_check' => true,
        'package_length_type' => 'N',
        'package_length_offset' => 0,
        'package_body_offset' => 0,
    ]);
    $serv->on('workerStart', function (Server $serv, $wid) use ($pm) {
        if ($wid === 0) {
            $pm->wakeup();
        }
    });
    $serv->on('receive', function (Server $serv, $fd, $tid, $recv) {
        list($op, $args) = opcode_decode($recv);
        if ($op === 'stats') {
            $serv->send($fd, opcode_encode("return", $serv->stats()));
        }
    });
    $serv->on('task', function () {});
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
