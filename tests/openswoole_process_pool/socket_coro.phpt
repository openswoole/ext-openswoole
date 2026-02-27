--TEST--
openswoole_process_pool: co\socket
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use SwooleTest\ProcessManager;
use OpenSwoole\Coroutine\Socket;
use OpenSwoole\Constant;
use OpenSwoole\Process;

$pm = new ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {

    $s = microtime(true);
    $sch = new OpenSwoole\Coroutine\Scheduler();
    $sch->parallel(2, function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        if (!$cli->connect('127.0.0.1', $pm->getFreePort())) {
            echo "ERROR\n";
            return;
        }
        if (!$cli->send("hello\n")) {
            return;
        }
        $ret = $cli->recv();
        if (!$ret) {
            return;
        }
        echo $ret;
    });
    $sch->start();
    echo "DONE\n";
    Assert::lessThan(microtime(true) - $s, 0.15);
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $socket = new Socket(AF_INET, SOCK_STREAM, 0);
    $socket->bind('127.0.0.1', $pm->getFreePort());

    $atomic = new \OpenSwoole\Atomic();

    $pool = new OpenSwoole\Process\Pool(2);
    $pool->set(['enable_coroutine' => true]);
    $pool->on(Constant::EVENT_WORKER_START, function ($pool, $id) use ($socket, $pm, $atomic) {
        $socket->listen(128);
        if ($atomic->add() == 2) {
            $pm->wakeup();
        }
        Process::signal(SIGTERM, function () use ($socket) {
            $socket->cancel();
        });
        while (true) {
            $client = $socket->accept();
            if (!$client) {
                if ($socket->errCode == SOCKET_ECANCELED) {
                    break;
                }
                continue;
            }
            usleep(100000);
            $data = $client->recv();
            if (empty($data)) {
                $client->close();
                break;
            }
            $client->send("Server[$id]: $data");
        }
        echo "worker stop\n";
    });

    $pool->start();
};

$pm->childFirst();
$pm->run();

?>
--EXPECTF--
Server[%d]: hello
Server[%d]: hello
DONE
worker stop
worker stop
