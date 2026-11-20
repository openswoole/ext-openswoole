--TEST--
openswoole_server: dispatch_mode = 5 [uid modulo] and bind
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Atomic;
use OpenSwoole\Coroutine\Client;
use OpenSwoole\Server;

const WORKER_N = 4;
const EOF = "\r\n";
const UIDS = [5, 6, 7, 8, 4294967295];
const REQ_N = 5;

$started = new Atomic(0);

function request(Client $cli, string $cmd): array
{
    Assert::assert($cli->send($cmd . EOF));
    $data = $cli->recv(5);
    Assert::notEmpty($data, "errCode={$cli->errCode}");
    return json_decode(trim($data), true);
}

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        foreach (UIDS as $uid) {
            go(function () use ($pm, $uid) {
                $cli = new Client(SWOOLE_SOCK_TCP);
                $cli->set([
                    'open_eof_split' => true,
                    'package_eof' => EOF,
                ]);
                Assert::true($cli->connect(TCP_SERVER_HOST, $pm->getFreePort(), 5));

                // not bound yet, uid is 0
                $r = request($cli, 'info');
                Assert::same($r['uid'], 0);

                $r = request($cli, "bind $uid");
                Assert::true($r['bind']);
                Assert::same($r['uid'], $uid);
                // a connection is bound once
                $r = request($cli, 'bind 1');
                Assert::false($r['bind']);
                Assert::same($r['uid'], $uid);

                for ($i = 0; $i < REQ_N; $i++) {
                    $r = request($cli, 'info');
                    Assert::same($r['worker'], $uid % WORKER_N, "uid $uid");
                    Assert::same($r['uid'], $uid);
                }
                $cli->close();
            });
        }
    });
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm, $started) {
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(), Server::POOL_MODE);
    $serv->set([
        'worker_num' => WORKER_N,
        'dispatch_mode' => 5,
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
        $cmd = explode(' ', trim($data));
        $reply = ['fd' => $fd, 'worker' => $serv->getWorkerId()];
        if ($cmd[0] == 'bind') {
            $reply['bind'] = $serv->bind($fd, (int) $cmd[1]);
            // an unknown connection and a uid out of range are refused
            Assert::false($serv->bind(999999, 1));
            Assert::false(@$serv->bind($fd, 4294967296));
        }
        $reply['uid'] = $serv->getClientInfo($fd)['uid'];
        $serv->send($fd, json_encode($reply) . EOF);
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
