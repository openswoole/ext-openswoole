--TEST--
openswoole_server: (eof protocol) eof split across writes and several packets in one write
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Coroutine\System;
use OpenSwoole\Server;

const EOF = "\r\n\r\n";

// send each part as its own write and wait for the expected replies
function exchange(Client $cli, array $writes, string $expect): void
{
    foreach ($writes as $i => $w) {
        if ($i > 0) {
            // give the server a chance to read the previous part on its own
            System::usleep(20000);
        }
        Assert::same($cli->send($w), strlen($w));
    }
    $data = '';
    while (strlen($data) < strlen($expect)) {
        $r = $cli->recv(5);
        Assert::notEmpty($r);
        $data .= $r;
    }
    Assert::same($data, $expect);
}

$pm = new SwooleTest\ProcessManager;
$pm->initFreePorts(2);
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $splits = [
            [["hello\r\n", "\r\n"], "[hello\r\n\r\n]"],
            [["abc\r", "\n\r\n"], "[abc\r\n\r\n]"],
            [["xyz\r\n\r", "\n"], "[xyz\r\n\r\n]"],
            [["\r", "\n", "\r", "\n"], "[\r\n\r\n]"],
        ];

        // without open_eof_split
        $cli = new Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(0), 1));
        foreach ($splits as [$writes, $expect]) {
            exchange($cli, $writes, $expect);
        }
        // the data is passed as it ends with the eof, not split
        exchange($cli, ["a\r\n\r\nbb\r\n\r\n"], "[a\r\n\r\nbb\r\n\r\n]");
        $cli->close();

        // with open_eof_split
        $cli = new Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(1), 1));
        foreach ($splits as [$writes, $expect]) {
            exchange($cli, $writes, $expect);
        }
        exchange($cli, ["a\r\n\r\nbb\r\n\r\nccc\r\n\r\n"], "[a\r\n\r\n][bb\r\n\r\n][ccc\r\n\r\n]");
        // complete packets followed by a partial one
        exchange($cli, ["a\r\n\r\nbb\r\n\r\ncc", "c\r\n\r\n"], "[a\r\n\r\n][bb\r\n\r\n][ccc\r\n\r\n]");
        // the eof of the second packet is split
        exchange($cli, ["e\r\n\r\nf\r\n", "\r\ng\r\n\r\n"], "[e\r\n\r\n][f\r\n\r\n][g\r\n\r\n]");
        // many packets in one write
        $pkts = '';
        $expect = '';
        for ($i = 0; $i < 200; $i++) {
            $p = str_repeat(chr(ord('a') + $i % 26), $i + 1) . EOF;
            $pkts .= $p;
            $expect .= "[$p]";
        }
        exchange($cli, [$pkts], $expect);
        $cli->close();
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(0), Server::POOL_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'open_eof_check' => true,
        'package_eof' => EOF,
    ]);
    $port = $serv->listen(TCP_SERVER_HOST, $pm->getFreePort(1), SWOOLE_SOCK_TCP);
    $port->set([
        'open_eof_split' => true,
        'package_eof' => EOF,
    ]);
    $serv->on('WorkerStart', function (Server $serv) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {
        $serv->send($fd, "[$data]");
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
