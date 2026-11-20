--TEST--
openswoole_server: responses and task data larger than one ipc datagram in process mode
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $base = "http://127.0.0.1:{$pm->getFreePort()}";
        foreach ([100, 1854, 4000, 100000] as $size) {
            Assert::same(strlen(httpGetBody("$base/?size=$size")), $size);
        }
        foreach ([1000, 5000, 100000] as $size) {
            Assert::same(httpGetBody("$base/?task=1&size=$size"), (string) $size);
        }
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $http->set([
        'worker_num' => 1,
        'task_worker_num' => 1,
        'log_file' => '/dev/null',
    ]);
    $http->on('workerStart', function ($server) use ($pm) {
        if (!$server->taskworker) {
            $pm->wakeup();
        }
    });
    $http->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) use ($http) {
        $size = (int) $request->get['size'];
        if (isset($request->get['task'])) {
            $response->end((string) strlen((string) $http->taskwait(str_repeat('t', $size), 5)));
            return;
        }
        $response->end(str_repeat('a', $size));
    });
    $http->on('task', function ($server, $taskId, $workerId, $data) {
        return $data;
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
