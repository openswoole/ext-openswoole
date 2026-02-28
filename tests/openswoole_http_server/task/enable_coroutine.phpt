--TEST--
openswoole_http_server/task: use async io and coroutine in task process
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
$randoms = [];
for ($n = MAX_REQUESTS; $n--;) {
    $randoms[] = get_safe_random(mt_rand(0, 65536));
}
$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        for ($n = MAX_REQUESTS; $n--;) {
            if (!Assert::assert(($res = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/task?n={$n}")) === 'OK')) {
                echo "{$res}\n";
                break;
            }
        }
    });
    OpenSwoole\Event::wait();
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $server = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $server->set([
        'log_file' => '/dev/null',
        'task_worker_num' => 1,
        'task_enable_coroutine' => true
    ]);
    $server->on('workerStart', function ($serv, $wid) use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($server) {
        global $randoms;
        $n = $request->get['n'];
        switch ($request->server['path_info']) {
            case '/task':
                {
                    list($ret_n, $ret_random) = $server->taskCo([$n], 1)[0];
                    if ($ret_n !== $n) {
                        $response->end("ERROR MATCH {$ret_n} with {$n}");
                        return;
                    } elseif ($ret_random !== $randoms[$n]) {
                        $response->end("ERROR EQUAL {$ret_n}(" . strlen($ret_random) . ") with {$n}(" . strlen($randoms[$n]) . ")");
                        return;
                    }
                    $response->end('OK');
                    break;
                }
            case '/random':
                {
                    $response->end($randoms[$n]);
                    break;
                }
        }
    });
    $server->on('task', function (openswoole_http_server $server, openswoole_server_task $task) use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        $cli->get("/random?n={$task->data}");
        $task->finish([$task->data, $cli->body]);
    });
    $server->on('finish', function () { });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
