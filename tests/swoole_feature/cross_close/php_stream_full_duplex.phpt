--TEST--
swoole_feature/cross_close: full duplex (php stream)
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
Swoole\Runtime::enableCoroutine();
$pm = new ProcessManager();
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = stream_socket_client("tcp://127.0.0.1:{$pm->getFreePort()}", $errno, $errstr, 1);
        Assert::true(!$errno);
        go(function () use ($pm, $cli) {
            Co::usleep(1000);
            echo "CLOSE\n";
            Assert::true(fclose($cli));
            $pm->kill();
            echo "DONE\n";
        });
        go(function () use ($cli) {
            echo "SEND\n";
            $size = 64 * 1024 * 1024;
            Assert::true(@fwrite($cli, str_repeat('S', $size)) < $size);
            echo "SEND CLOSED\n";
        });
        go(function () use ($cli) {
            echo "RECV\n";
            Assert::true(empty(fread($cli, 8192)));
            echo "RECV CLOSED\n";
        });
    });
};
$pm->childFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $server = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        Assert::true($server->bind('127.0.0.1', $pm->getFreePort()));
        Assert::true($server->listen());
        go(function () use ($pm, $server) {
            if (Assert::true(($conn = $server->accept()) && $conn instanceof OpenSwoole\Coroutine\Socket)) {
                switch_process();
                co::sleep(5);
                $conn->close();
            }
            $server->close();
        });
        $pm->wakeup();
    });
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
SEND
RECV
CLOSE
SEND CLOSED
RECV CLOSED
DONE
