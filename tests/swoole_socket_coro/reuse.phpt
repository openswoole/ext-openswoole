--TEST--
swoole_socket_coro: reuse socket object
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const SEND_STR = "hello world\n";

use SwooleTest\ProcessManager;

$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
        echo "Co [1]\n";
        $map = [];
        go(function () use ($pm, &$map) {
            $socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
            Assert::assert($socket->connect('127.0.0.1', $pm->getFreePort()));
            Assert::assert($socket->send(SEND_STR));
            echo $socket->recv();
            $map['sock'] = $socket;
        });

        echo "Co [2]\n";

        go(function () use ($pm, &$map) {
            co::sleep(1);
            $socket = $map['sock'];
            Assert::assert($socket->send(SEND_STR));
            echo $socket->recv();
            unset($map['sock']);
        });

        OpenSwoole\Event::wait();
};

$pm->childFunc = function () use ($pm) {
    $socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    Assert::assert($socket->bind('127.0.0.1', $pm->getFreePort()));
    Assert::assert($socket->listen(128));
    $pm->wakeup();
    go(function () use ($socket, $pm) {
        $client = $socket->accept();
        Assert::isInstanceOf($client, OpenSwoole\Coroutine\Socket::class);
        while (true) {
            $client_data = $client->recv(1024, -1);
            if (empty($client_data)) {
                echo "closed\n";
                break;
            }
            if ($client->errCode > 0) {
                Assert::same($client->errCode, SOCKET_ETIMEDOUT);
                break;
            } else {
                Assert::same($client_data, SEND_STR);
                $client->send('swoole '.SEND_STR);
            }
        }
        $client->close();
        $socket->close();
        echo "server exit\n";
    });
    OpenSwoole\Event::wait();

};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
Co [1]
Co [2]
swoole hello world
swoole hello world
closed
server exit
