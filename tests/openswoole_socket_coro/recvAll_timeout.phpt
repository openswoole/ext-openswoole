--TEST--
openswoole_socket_coro: recvAll timeout (slow network)
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->initRandomDataEx(MAX_CONCURRENCY_MID, 1, 1024);
$pm->parentFunc = function ($pid) use ($pm) {
    for ($c = MAX_CONCURRENCY_MID; $c--;) {
        go(function () use ($pm, $c) {
            $conn = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
            Assert::assert($conn->connect('127.0.0.1', $pm->getFreePort()));
            $conn->send((string)$c);
            $timeout = ms_random(0.1, 1);
            $s = microtime(true);
            $data = $conn->recvAll(1024, $timeout);
            time_approximate($timeout, microtime(true) - $s);
            Assert::assert(strlen($data) > 0);
            Assert::assert(strlen($data) != 1024);
            Assert::assert(strpos($pm->getRandomDataEx($c), $data) === 0);
            Assert::assert($conn->errCode == SOCKET_ETIMEDOUT);
            Assert::assert($conn->errMsg == OpenSwoole\Util::getErrorMessage(SOCKET_ETIMEDOUT));
        });
    }
    OpenSwoole\Event::wait();
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('Receive', function (OpenSwoole\Server $server, int $fd, int $rid, string $data) use ($pm) {
        $s = $pm->getRandomDataEx((int)$data);
        while ($server->exists($fd)) {
            $server->send($fd, string_pop_front($s, 1));
            Co::usleep(5000);
        }
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
