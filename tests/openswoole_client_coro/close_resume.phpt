--TEST--
openswoole_client_coro: (length protocol) resume in onClose callback
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm)
{
    go(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        $cli->set([
            'open_length_check' => true,
            'package_max_length' => 1024 * 1024,
            'package_length_type' => 'N',
            'package_length_offset' => 0,
            'package_body_offset' => 4,
        ]);
        $cli->connect('127.0.0.1', $pm->getFreePort());
        $data = str_repeat('A', 1025);
        $cli->send(pack('N', strlen($data)).$data);
        co::usleep(200000);
        $retData = $cli->recv();
        Assert::assert(is_string($retData) and strlen($retData) > 0);
        $retData = $cli->recv();
        Assert::same($retData, '');
    });
    openswoole_event_wait();
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new openswoole_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $serv->set([
        'worker_num' => 1,
        //'dispatch_mode'         => 1,
        'log_file' => '/dev/null',
        'open_length_check' => true,
        'package_max_length' => 1024 * 1024,
        'package_length_type' => 'N',
        'package_length_offset' => 0,
        'package_body_offset' => 4,
    ]);
    $serv->on("WorkerStart", function (\openswoole_server $serv)  use ($pm)
    {
        $pm->wakeup();
    });
    $serv->on('receive', function (openswoole_server $serv, $fd, $rid, $data)
    {
        $data = str_repeat('B', 1025);
        $serv->send($fd, pack('N', strlen($data)) . $data);
        $serv->close($fd);
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
