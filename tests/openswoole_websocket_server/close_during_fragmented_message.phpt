--TEST--
openswoole_websocket_server: closing a connection during a fragmented message releases its buffer
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_darwin();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const CONNECTIONS = 40;
const FRAGMENT = 1024 * 1024;

function rss_kb(int $pid): int
{
    preg_match('/^VmRSS:\s+(\d+)/m', file_get_contents("/proc/$pid/status"), $m);
    return (int) $m[1];
}

function masked_frame(int $opcode_byte, string $payload): string
{
    $len = strlen($payload);
    $header = chr($opcode_byte) . ($len < 126 ? chr(0x80 | $len) : ($len < 65536 ? chr(0x80 | 126) . pack('n', $len) : chr(0x80 | 127) . pack('J', $len)));
    $mask = random_bytes(4);
    return $header . $mask . ($payload ^ str_repeat($mask, intdiv($len, 4) + 1));
}

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function (int $pid) use ($pm) {
    $worker = (int) trim(file_get_contents('/tmp/openswoole_ws_fragment_worker.pid'));
    $upgrade = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\n" .
        "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n";
    $round = function () use ($pm, $upgrade) {
        co::run(function () use ($pm, $upgrade) {
            for ($i = 0; $i < CONNECTIONS; $i++) {
                $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
                Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 1));
                $cli->send($upgrade);
                Assert::contains($cli->recv(2), '101 Switching Protocols');
                // a text frame without FIN, the message is never finished
                $cli->send(masked_frame(0x01, str_repeat('A', FRAGMENT)));
                co::usleep(5000);
                $cli->close();
            }
        });
        usleep(300000);
    };
    $round();
    $before = rss_kb($worker);
    $round();
    $round();
    $growth = rss_kb($worker) - $before;
    // without releasing the buffers two rounds keep about 80 MiB
    Assert::lessThan($growth, 20 * 1024);
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::SIMPLE_MODE);
    $serv->set(['worker_num' => 1, 'log_file' => '/dev/null', 'package_max_length' => 4 * 1024 * 1024]);
    $serv->on('WorkerStart', function () use ($pm) {
        file_put_contents('/tmp/openswoole_ws_fragment_worker.pid', getmypid());
        $pm->wakeup();
    });
    $serv->on('Message', function () {
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--CLEAN--
<?php @unlink('/tmp/openswoole_ws_fragment_worker.pid'); ?>
--EXPECT--
DONE
