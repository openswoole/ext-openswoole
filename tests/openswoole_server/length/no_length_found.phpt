--TEST--
openswoole_server/length: package_length_func returning 0 for a full header closes the connection
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc';
skip('not support ZTS', ZEND_THREAD_SAFE);
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Atomic;
use OpenSwoole\Coroutine\Client;
use OpenSwoole\Coroutine\System;
use OpenSwoole\Server;

// the longest data the length function was called with
$seen = new Atomic(0);

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm, $seen) {
    co::run(function () use ($pm, $seen) {
        $cli = new Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 1));
        // part of the header, the server waits for more
        Assert::same($cli->send(str_repeat('A', 1000)), 1000);
        for ($i = 0; $i < 500 && $seen->get() < 1000; $i++) {
            System::usleep(10000);
        }
        Assert::same($seen->get(), 1000);
        Assert::true($cli->isConnected());
        // complete the header, the length function still finds no length
        $cli->send(str_repeat('A', 16 * 1024));
        $r = $cli->recv(5);
        // unread bytes make the kernel reset the connection instead of a clean close
        Assert::true($r === '' || ($r === false && in_array($cli->errCode, [54, 104])), "errCode={$cli->errCode}");
        $cli->close();
        // the header is what is read before the length function gives up
        $header = $seen->get();
        Assert::greaterThan($header, 1000);
        Assert::lessThan($header, 8192);

        $cli = new Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 1));
        $cli->send('ping');
        Assert::same($cli->recv(5), 'pong');
        $cli->close();

        $log = file_get_contents(TEST_LOG_FILE);
        Assert::contains($log, "no length found in {$header} bytes");
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm, $seen) {
    @unlink(TEST_LOG_FILE);
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(), Server::POOL_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => TEST_LOG_FILE,
        'open_length_check' => true,
        'package_length_func' => function (string $data) use ($seen) {
            if ($data === 'ping') {
                return 4;
            }
            if (strlen($data) > $seen->get()) {
                $seen->set(strlen($data));
            }
            return 0;
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
@unlink(TEST_LOG_FILE);
?>
--EXPECT--
DONE
