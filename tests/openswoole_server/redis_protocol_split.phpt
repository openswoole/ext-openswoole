--TEST--
openswoole_server: open_redis_protocol handles commands split across reads and pipelined commands
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        // split after a complete line
        $cli->send("*2\r\n\$3\r\nGET\r\n");
        co::usleep(100000);
        $cli->send("\$3\r\nkey\r\n");
        // two commands in one write
        $cli->send("*1\r\n\$4\r\nPING\r\n*2\r\n\$4\r\nECHO\r\n\$2\r\nhi\r\n");
        $data = '';
        while (substr_count($data, "\n") < 3 && ($chunk = $cli->recv(3))) {
            $data .= $chunk;
        }
        echo $data;
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_redis_protocol' => true]);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('receive', function ($server, $fd, $rid, $data) {
        // one command per receive
        $server->send($fd, str_replace("\r\n", ' ', trim($data)) . "\n");
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
*2 $3 GET $3 key
*1 $4 PING
*2 $4 ECHO $2 hi
