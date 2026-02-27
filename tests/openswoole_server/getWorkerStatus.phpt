--TEST--
openswoole_server: getWorkerStatus
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::assert($client->connect('127.0.0.1', $pm->getFreePort()));
        Assert::assert($client->send('world'));

        $data = $client->recv();
        Assert::assert($data);
        $json = json_decode($data);
        Assert::assert($json);

        Assert::eq($json->current_worker, SWOOLE_WORKER_BUSY);
        Assert::eq($json->another_worker, SWOOLE_WORKER_IDLE);

        $pm->kill();
    });
};
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort());
    $server->set([
        'worker_num' => 2,
        'log_file' => '/dev/null'
    ]);
    $server->on('workerStart', function (OpenSwoole\Server $serv) use ($pm) {
        $pm->wakeup();
    });

    $server->on('receive', function (OpenSwoole\Server $serv, int $fd, int $rid, string $data) {
        $serv->send($fd, json_encode([
            'current_worker' => $serv->getWorkerStatus(), 
            'another_worker' => $serv->getWorkerStatus($serv->getWorkerId() == 0 ? 1 : 0),
        ]));
    });

    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
