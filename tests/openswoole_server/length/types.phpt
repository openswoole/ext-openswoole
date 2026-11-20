--TEST--
openswoole_server/length: every package_length_type with length and body offsets
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Server;

const MAX_LENGTH = 256 * 1024;
const PREFIX = 'abc';

// type => [size, body length that needs the full range of the type]
const TYPES = [
    'c' => [1, 100],
    'C' => [1, 200],
    's' => [2, 30000],
    'S' => [2, 40000],
    'n' => [2, 40000],
    'v' => [2, 40000],
    'l' => [4, 70000],
    'L' => [4, 70000],
    'N' => [4, 70000],
    'V' => [4, 70000],
];

function new_client(int $port): Client
{
    $cli = new Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect(TCP_SERVER_HOST, $port, 1));
    return $cli;
}

function packet(string $type, int $len, string $char): string
{
    // prefix, length field, one padding byte, body
    return PREFIX . pack($type, $len) . '#' . str_repeat($char, $len);
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
$pm->initFreePorts(count(TYPES));
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $i = 0;
        foreach (TYPES as $type => [$size, $len]) {
            $port = $pm->getFreePort($i++);
            $cli = new_client($port);
            // two packets in one write, then a small one
            $pkts = [packet($type, $len, 'A'), packet($type, 1, 'B'), packet($type, 0, 'C')];
            $all = implode('', $pkts);
            Assert::same($cli->send($all), strlen($all));
            $data = '';
            while (strlen($data) < strlen($all)) {
                $r = $cli->recv(5);
                Assert::notEmpty($r, "type $type");
                $data .= $r;
            }
            Assert::same($data, $all, "type $type");
            $cli->close();

            if (in_array($type, ['c', 's', 'l'])) {
                // a negative signed length
                assert_closed(new_client($port), PREFIX . pack($type, -1) . '#AAAA');
            } elseif ($size == 4) {
                // an unsigned 32 bit length of 2^31 and more
                assert_closed(new_client($port), PREFIX . pack($type, 0x80000001) . '#AAAA');
            }

            $cli = new_client($port);
            $pkt = packet($type, 10, 'D');
            $cli->send($pkt);
            Assert::same($cli->recv(5), $pkt, "type $type");
            $cli->close();
        }
    });
    $pm->kill();
    $log = file_get_contents(TEST_LOG_FILE);
    Assert::same(substr_count($log, 'invalid package'), 3);
    Assert::same(substr_count($log, 'package is too big'), 3);
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    @unlink(TEST_LOG_FILE);
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(0), Server::POOL_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => TEST_LOG_FILE,
    ]);
    $i = 0;
    foreach (TYPES as $type => [$size, $len]) {
        $port = $i == 0 ? $serv->ports[0] : $serv->listen(TCP_SERVER_HOST, $pm->getFreePort($i), SWOOLE_SOCK_TCP);
        $port->set([
            'open_length_check' => true,
            'package_max_length' => MAX_LENGTH,
            'package_length_type' => $type,
            'package_length_offset' => strlen(PREFIX),
            'package_body_offset' => strlen(PREFIX) + $size + 1,
        ]);
        $i++;
    }
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
