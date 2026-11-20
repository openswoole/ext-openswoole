--TEST--
openswoole_server: open_redis_protocol handles null bulk strings, integers and empty arrays
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
        $cli->send("*1\r\n\$-1\r\n");
        $cli->send("*0\r\n");
        $cli->send("*3\r\n\$3\r\nSET\r\n:5\r\n\$-1\r\n");
        // a one byte first read
        co::usleep(100000);
        $cli->send('*');
        co::usleep(100000);
        $cli->send("1\r\n\$4\r\nPING\r\n");
        $data = '';
        while (substr_count($data, "\n") < 4 && ($chunk = $cli->recv(3))) {
            $data .= $chunk;
        }
        echo $data;
        // a malformed length is rejected
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $cli->send("*1\r\n\$\r\n4\r\nPING\r\n");
        var_dump($cli->recv(3));
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
*1 $-1
*0
*3 $3 SET :5 $-1
*1 $4 PING
string(0) ""
