--TEST--
openswoole_websocket_server: frame flags are kept per connection with several reactor threads
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

const CLIENTS = 8;
const BATCHES = 40;
const FRAMES_PER_BATCH = 500;

// masked frame with a zero mask key, so the payload is sent as is
function frame(int $opcode, string $payload): string
{
    return chr(0x80 | $opcode) . chr(0x80 | strlen($payload)) . "\0\0\0\0" . $payload;
}

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        for ($c = 0; $c < CLIENTS; $c++) {
            go(function () use ($pm, $c) {
                $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
                Assert::true($cli->connect('127.0.0.1', $pm->getFreePort(), 5));
                $cli->send("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\n" .
                    "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n");
                $data = '';
                while (!str_contains($data, "\r\n\r\n")) {
                    $chunk = $cli->recv(5);
                    Assert::notEmpty($chunk);
                    $data .= $chunk;
                }
                Assert::contains($data, '101 Switching Protocols');
                // text and binary frames on different connections
                $opcode = $c % 2 ? WEBSOCKET_OPCODE_TEXT : WEBSOCKET_OPCODE_BINARY;
                $batch = str_repeat(frame($opcode, (string) $opcode), FRAMES_PER_BATCH);
                for ($i = 0; $i < BATCHES; $i++) {
                    Assert::same($cli->send($batch), strlen($batch));
                }
                $cli->send(frame(WEBSOCKET_OPCODE_TEXT, 'end'));
                $frames = substr($data, strpos($data, "\r\n\r\n") + 4);
                while (strlen($frames) < 2 || strlen($frames) < 2 + ord($frames[1])) {
                    $chunk = $cli->recv(10);
                    Assert::notEmpty($chunk);
                    $frames .= $chunk;
                }
                Assert::same(substr($frames, 2, ord($frames[1])), (BATCHES * FRAMES_PER_BATCH) . ' 0');
                $cli->close();
            });
        }
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::POOL_MODE);
    $serv->set(['reactor_num' => 4, 'worker_num' => 4, 'log_file' => '/dev/null']);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
        static $count = [], $bad = [];
        $fd = $frame->fd;
        if ($frame->data === 'end') {
            $serv->push($fd, ($count[$fd] ?? 0) . ' ' . ($bad[$fd] ?? 0));
            return;
        }
        $count[$fd] = ($count[$fd] ?? 0) + 1;
        if (!$frame->finish || $frame->opcode !== (int) $frame->data) {
            $bad[$fd] = ($bad[$fd] ?? 0) + 1;
        }
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
