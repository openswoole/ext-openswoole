--TEST--
openswoole_server: dispatch_mode = 3
--CONFLICTS--
all
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
const WORKER_N = 16;

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Timer;
use OpenSwoole\Event;
use OpenSwoole\Server;

global $stats;
$stats = array();
$count = 0;
$port = get_one_free_port();

$pm = new SwooleTest\ProcessManager;
$started = new OpenSwoole\Atomic(0);
$pm->parentFunc = function ($pid) use ($port) {
    global $count, $stats;
    for ($i = 0; $i < MAX_CONCURRENCY; $i++) {
        go(function () use ($port) {
            $cli = new Client(SWOOLE_SOCK_TCP);
            $cli->set([
                'package_eof' => "\r\n\r\n",
                'open_eof_split' => true,
            ]);
            // the connect timeout is also the read timeout, it must be longer than the 1s the slow workers sleep
            $r = $cli->connect(TCP_SERVER_HOST, $port, 10);
            Assert::assert($r);
            // one request at a time: a worker only reports busy once it handles a request,
            // so a burst could still land on the slow workers
            for ($i = 0; $i < MAX_REQUESTS; $i++) {
                Assert::assert($cli->send("hello world\r\n\r\n"));
                $data = $cli->recv();
                Assert::assert($data !== false, "recv failed, errCode={$cli->errCode}");
                global $stats;
                $wid = trim($data);
                $stats[$wid] = ($stats[$wid] ?? 0) + 1;
            }
            $cli->close();
        });
    }
    Event::wait();
    OpenSwoole\Process::kill($pid);
    phpt_var_dump($stats);
    Assert::eq(count($stats), WORKER_N);
    Assert::lessThan($stats[5], MAX_REQUESTS);
    Assert::lessThan($stats[10], MAX_REQUESTS);
    Assert::same(array_sum($stats), MAX_REQUESTS * MAX_CONCURRENCY);
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm, $port, $started) {
    $serv = new Server('127.0.0.1', $port, SWOOLE_PROCESS);
    $serv->set(array(
        "worker_num" => WORKER_N,
        'dispatch_mode' => 3,
        'package_eof' => "\r\n\r\n",
        'enable_coroutine' => false,
        'open_eof_split' => true,
        'log_file' => '/dev/null',
    ));
    $serv->on("WorkerStart", function (Server $serv) use ($pm, $started) {
        // every worker has to be running, the test checks that all of them get requests
        if ($started->add() == WORKER_N) {
            $pm->wakeup();
        }
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {
        if ($serv->worker_id == 10 or $serv->worker_id == 5) {
            sleep(1);
        }
        $serv->send($fd, $serv->worker_id . "\r\n\r\n");
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
