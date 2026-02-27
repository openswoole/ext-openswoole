--TEST--
openswoole_server: discard timeout packet
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Timer;
use OpenSwoole\Event;
use OpenSwoole\Server;

const TMP_LOG_FILE = '/tmp/swoole.server.log';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $n = 2;
        while($n--) {
            $client = new Client(SWOOLE_SOCK_TCP);
            $client->set([
                'open_eof_check' => true,
                'open_eof_split' => true,
                "package_eof" => "\r\n",
            ]);
            $client->connect('127.0.0.1', $pm->getFreePort());
            $client->send("OpenSwoole\r\nhello world\r\nphp\r\njava\r\n");
            $client->close();
        }
        Co::usleep(800000);
    });
    $pm->kill();
    Assert::eq(substr_count(file_get_contents(TMP_LOG_FILE),
        'Worker_discard_data() (ERRNO 1007): [2] ignore data'), 8);
    unlink(TMP_LOG_FILE);
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $serv->set([
        "package_eof" => "\r\n",
        'open_eof_check' => true,
        'open_eof_split' => true,
        'dispatch_mode' => 3,
        'discard_timeout_request' => true,
        "worker_num" => 1,
        'log_file' => TMP_LOG_FILE,
    ]);
    $serv->on('workerStart', function (Server $serv) use ($pm) {
        $pm->wakeup();
        co::usleep(500000);
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {
        $serv->send($fd, "hello {$data}\r\n\r\n");
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
