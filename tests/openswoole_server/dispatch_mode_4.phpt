--TEST--
openswoole_server: dispatch_mode = 4 [ip modulo]
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
// connects from other loopback addresses
skip_if_darwin();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Atomic;
use OpenSwoole\Coroutine\Client;
use OpenSwoole\Server;

const WORKER_N = 3;
const EOF = "\r\n";
const IPS = ['127.0.0.1', '127.0.0.2', '127.0.0.3'];
const CONN_N = 4;
const REQ_N = 5;

// the ipv4 address as stored in the connection, in machine byte order
function expected_worker(string $ip): int
{
    return unpack('L', inet_pton($ip))[1] % WORKER_N;
}

$started = new Atomic(0);

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    $workers = array_map('expected_worker', IPS);
    // the addresses cover every worker
    Assert::same(count(array_unique($workers)), WORKER_N);
    $result = [];
    co::run(function () use ($pm, &$result) {
        foreach (IPS as $ip) {
            for ($c = 0; $c < CONN_N; $c++) {
                go(function () use ($pm, $ip, &$result) {
                    $cli = new Client(SWOOLE_SOCK_TCP);
                    $cli->set([
                        'bind_address' => $ip,
                        'open_eof_split' => true,
                        'package_eof' => EOF,
                    ]);
                    Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 5));
                    for ($i = 0; $i < REQ_N; $i++) {
                        Assert::assert($cli->send('who' . EOF));
                        $data = $cli->recv(5);
                        Assert::notEmpty($data, "errCode={$cli->errCode}");
                        [$wid, $peer] = explode(' ', trim($data));
                        Assert::same($peer, $ip);
                        $result[$ip][(int) $wid] = ($result[$ip][(int) $wid] ?? 0) + 1;
                    }
                    $cli->close();
                });
            }
        }
    });
    $pm->kill();
    foreach (IPS as $ip) {
        // every request from one address goes to the same worker
        Assert::same($result[$ip], [expected_worker($ip) => CONN_N * REQ_N], $ip);
    }
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm, $started) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(), Server::POOL_MODE);
    $serv->set([
        'worker_num' => WORKER_N,
        'dispatch_mode' => 4,
        'log_file' => '/dev/null',
        'open_eof_split' => true,
        'package_eof' => EOF,
    ]);
    $serv->on('WorkerStart', function (Server $serv) use ($pm, $started) {
        if ($started->add() == WORKER_N) {
            $pm->wakeup();
        }
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {
        $info = $serv->getClientInfo($fd);
        $serv->send($fd, $serv->getWorkerId() . ' ' . $info['remote_ip'] . EOF);
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
