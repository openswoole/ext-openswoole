--TEST--
openswoole_server: buffer_high_watermark set by the application is used
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Server;

$full = new OpenSwoole\Atomic(0);
$empty = new OpenSwoole\Atomic(0);
$pm = new SwooleTest\ProcessManager;

$pm->parentFunc = function () use ($pm, $full, $empty) {
    co::run(function () use ($pm, $full, $empty) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 1));
        // do not read until the server output buffer is above 1 MiB
        for ($i = 0; $i < 50 && $full->get() === 0; $i++) {
            co::usleep(100000);
        }
        Assert::same($full->get(), 1);
        $total = 0;
        while ($total < 8 * 1024 * 1024) {
            $data = $cli->recv(5);
            Assert::notEmpty($data);
            $total += strlen($data);
        }
        for ($i = 0; $i < 50 && $empty->get() === 0; $i++) {
            co::usleep(100000);
        }
        Assert::same($empty->get(), 1);
        $cli->close();
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm, $full, $empty) {
    $serv = new Server('127.0.0.1', $pm->getFreePort(), Server::SIMPLE_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        // the default high watermark would be 80% of this, far above what is sent
        'socket_buffer_size' => 32 * 1024 * 1024,
        'buffer_high_watermark' => 1024 * 1024,
        'buffer_low_watermark' => 0,
    ]);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Connect', function (Server $serv, int $fd) {
        $chunk = str_repeat('A', 64 * 1024);
        for ($i = 0; $i < 128; $i++) {
            $serv->send($fd, $chunk);
        }
    });
    $serv->on('BufferFull', function (Server $serv, int $fd) use ($full) {
        $full->add(1);
    });
    $serv->on('BufferEmpty', function (Server $serv, int $fd) use ($empty) {
        $empty->add(1);
    });
    $serv->on('Receive', function () {
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
