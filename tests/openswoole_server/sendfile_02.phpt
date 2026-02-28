--TEST--
openswoole_server: sendfile [02]
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_extension_not_exist('sockets');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm){
    $client = new openswoole_client(SWOOLE_SOCK_TCP , false); //同步阻塞
    if (!$client->connect('127.0.0.1', $pm->getFreePort())) {
        exit("connect failed\n");
    }

    $socket = $client->getSocket();
    socket_set_option($socket, SOL_SOCKET, SO_SNDBUF, 65536);
    socket_set_option($socket, SOL_SOCKET, SO_RCVBUF, 65536);

    $N = filesize(TEST_IMAGE);
    $bytes = 0;
    while($bytes < $N) {
        $n = rand(8192, 65536);
        $r = $client->recv($n);
        if (!$r) {
            break;
        }
        usleep(10000);
        $bytes += strlen($r);
    }
    Assert::same($bytes, $N);
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $serv = new openswoole_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE, SWOOLE_SOCK_TCP);
    $serv->set([
        'log_file' => '/dev/null',
        'kernel_socket_send_buffer_size' => 65536,
    ]);
    $serv->on("workerStart", function ($serv) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('connect', function (openswoole_server $serv, $fd) {
        Assert::true($serv->sendfile($fd, TEST_IMAGE));
    });
    $serv->on('receive', function ($serv, $fd, $reactor_id, $data) {

    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
