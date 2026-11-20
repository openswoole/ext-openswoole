--TEST--
openswoole_server/length: package_length_func that keeps returning 0
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
skip('not support ZTS', ZEND_THREAD_SAFE);
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 1));
        // small writes while the length function waits for more data
        for ($i = 0; $i < 2048; $i++) {
            if (!$cli->send(str_repeat('A', 64))) {
                break;
            }
            usleep(100);
        }
        // the server closes the connection
        Assert::same($cli->recv(5), '');
        $cli->close();

        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 1));
        $cli->send('ping');
        Assert::same($cli->recv(5), 'pong');
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(), Server::SIMPLE_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'open_length_check' => true,
        'package_length_func' => function (string $data) {
            // wait for more data, except for the health check
            return $data === 'ping' ? 4 : 0;
        },
    ]);
    $serv->on('WorkerStart', function (Server $serv) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function ($serv, $fd, $rid, $data) {
        $serv->send($fd, 'pong');
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
