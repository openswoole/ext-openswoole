--TEST--
openswoole_server/length: invalid and too big lengths close the connection
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Server;

// the server raises package_max_length to at least 64K
const MAX_LENGTH = 100000;

function new_client(int $port): Client
{
    $cli = new Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect(TCP_SERVER_HOST, $port, 1));
    return $cli;
}

function packet(int $len): string
{
    return pack('l', $len) . str_repeat('A', $len);
}

function assert_echo(Client $cli, string $pkt): void
{
    Assert::true($cli->send($pkt) === strlen($pkt));
    $data = '';
    while (strlen($data) < strlen($pkt)) {
        $r = $cli->recv(5);
        Assert::notEmpty($r);
        $data .= $r;
    }
    Assert::same($data, $pkt);
}

function assert_closed(Client $cli, string $pkt): void
{
    $cli->send($pkt);
    $r = $cli->recv(5);
    // unread bytes make the kernel reset the connection instead of a clean close
    Assert::true($r === '' || ($r === false && in_array($cli->errCode, [54, 104])), "errCode={$cli->errCode}");
    $cli->close();
}

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    $port = $pm->getFreePort();
    co::run(function () use ($port) {
        $cli = new_client($port);
        assert_echo($cli, packet(10));
        // a negative signed length
        assert_closed($cli, pack('l', -1) . 'A');

        $cli = new_client($port);
        assert_closed($cli, pack('l', 200000) . str_repeat('A', 16));

        // header plus body equal to package_max_length is accepted
        $cli = new_client($port);
        assert_echo($cli, packet(MAX_LENGTH - 4));
        assert_echo($cli, packet(1));
        // one byte more is rejected
        assert_closed($cli, packet(MAX_LENGTH - 3));

        $cli = new_client($port);
        assert_echo($cli, packet(100));
        $cli->close();
    });
    $pm->kill();
    $log = file_get_contents(TEST_LOG_FILE);
    Assert::same(substr_count($log, 'invalid package'), 1);
    Assert::same(substr_count($log, 'package is too big'), 2);
    Assert::contains($log, 'length=200004');
    Assert::contains($log, 'length=' . (MAX_LENGTH + 1));
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    @unlink(TEST_LOG_FILE);
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(), Server::POOL_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => TEST_LOG_FILE,
        'open_length_check' => true,
        'package_max_length' => MAX_LENGTH,
        'package_length_type' => 'l',
        'package_length_offset' => 0,
        'package_body_offset' => 4,
    ]);
    $serv->on('WorkerStart', function (Server $serv) use ($pm) {
        $pm->wakeup();
    });
    $serv->on('receive', function (Server $serv, $fd, $rid, $data) {
        $serv->send($fd, $data);
    });
    $serv->start();
};

$pm->childFirst();
$pm->run();
@unlink(TEST_LOG_FILE);
?>
--EXPECT--
DONE
