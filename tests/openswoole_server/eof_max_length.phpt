--TEST--
openswoole_server: (eof protocol) no eof before package_max_length closes the connection
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Server;

const EOF = "\r\n\r\n";
const MAX_LENGTH = 128 * 1024;

function new_client(int $port): Client
{
    $cli = new Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect(TCP_SERVER_HOST, $port, 1));
    return $cli;
}

function assert_echo(Client $cli, string $pkt): void
{
    Assert::same($cli->send($pkt), strlen($pkt));
    $data = '';
    while (strlen($data) < strlen($pkt)) {
        $r = $cli->recv(5);
        Assert::notEmpty($r);
        $data .= $r;
    }
    Assert::same($data, $pkt);
}

$pm = new SwooleTest\ProcessManager;
$pm->initFreePorts(2);
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        // port 0 without open_eof_split, port 1 with it
        for ($i = 0; $i < 2; $i++) {
            $port = $pm->getFreePort($i);
            $cli = new_client($port);
            // the largest packet that fits
            assert_echo($cli, str_repeat('A', MAX_LENGTH - strlen(EOF)) . EOF);
            // no eof in package_max_length bytes
            $cli->send(str_repeat('B', MAX_LENGTH + 4096));
            $r = $cli->recv(5);
            // unread bytes make the kernel reset the connection instead of a clean close
            Assert::true($r === '' || ($r === false && in_array($cli->errCode, [54, 104])), "errCode={$cli->errCode}");
            $cli->close();

            $cli = new_client($port);
            assert_echo($cli, 'hello' . EOF);
            $cli->close();
        }
    });
    $pm->kill();
    $log = file_get_contents(TEST_LOG_FILE);
    Assert::same(substr_count($log, 'Package is too big. package_length=' . MAX_LENGTH), 2);
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    @unlink(TEST_LOG_FILE);
    $serv = new Server(TCP_SERVER_HOST, $pm->getFreePort(0), Server::POOL_MODE);
    $serv->set([
        'worker_num' => 1,
        'log_file' => TEST_LOG_FILE,
        'open_eof_check' => true,
        'package_eof' => EOF,
        'package_max_length' => MAX_LENGTH,
    ]);
    $port = $serv->listen(TCP_SERVER_HOST, $pm->getFreePort(1), SWOOLE_SOCK_TCP);
    $port->set([
        'open_eof_split' => true,
        'package_eof' => EOF,
        'package_max_length' => MAX_LENGTH,
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
