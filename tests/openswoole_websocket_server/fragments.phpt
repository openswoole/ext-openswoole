--TEST--
openswoole_websocket_server: fragmented messages and control frames between fragments (POOL_MODE)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

function ws_frame(int $opcode, string $payload, bool $fin = true): string
{
    $len = strlen($payload);
    $head = chr(($fin ? 0x80 : 0) | $opcode);
    if ($len < 126) {
        $head .= chr(0x80 | $len);
    } elseif ($len < 65536) {
        $head .= chr(0x80 | 126) . pack('n', $len);
    } else {
        $head .= chr(0x80 | 127) . pack('J', $len);
    }
    $mask = random_bytes(4);
    return $head . $mask . ($payload ^ str_repeat($mask, intdiv($len + 3, 4)));
}

function ws_open(int $port): OpenSwoole\Coroutine\Client
{
    $cli = new OpenSwoole\Coroutine\Client(OpenSwoole\Constant::SOCK_TCP);
    Assert::true($cli->connect('127.0.0.1', $port, 5));
    $cli->send("GET / HTTP/1.1\r\nHost: localhost\r\nConnection: Upgrade\r\nUpgrade: websocket\r\n" .
        "Sec-WebSocket-Version: 13\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n\r\n");
    $data = '';
    while (!str_contains($data, "\r\n\r\n")) {
        $chunk = $cli->recv(5);
        Assert::notEmpty($chunk);
        $data .= $chunk;
    }
    Assert::contains($data, '101 Switching Protocols');
    Assert::same(substr($data, strpos($data, "\r\n\r\n") + 4), '');
    return $cli;
}

// reads one frame sent by the server, returns [opcode, payload] or null once the connection is closed
function ws_read_frame(OpenSwoole\Coroutine\Client $cli, string &$buf): ?array
{
    while (true) {
        if (strlen($buf) >= 2) {
            Assert::same(ord($buf[0]) & 0x80, 0x80);
            $opcode = ord($buf[0]) & 0x0f;
            $len = ord($buf[1]) & 0x7f;
            $pos = 2;
            if ($len === 126) {
                $pos = 4;
                $len = strlen($buf) >= $pos ? unpack('n', $buf, 2)[1] : PHP_INT_MAX;
            } elseif ($len === 127) {
                $pos = 10;
                $len = strlen($buf) >= $pos ? unpack('J', $buf, 2)[1] : PHP_INT_MAX;
            }
            if ($len !== PHP_INT_MAX && strlen($buf) >= $pos + $len) {
                $payload = substr($buf, $pos, $len);
                $buf = substr($buf, $pos + $len);
                return [$opcode, $opcode === 1 ? json_decode($payload, true) : $payload];
            }
        }
        $chunk = $cli->recv(5);
        if ($chunk === '' || $chunk === false) {
            if ($chunk === false) {
                Assert::notSame($cli->errCode, SOCKET_ETIMEDOUT);
            }
            Assert::same($buf, '');
            return null;
        }
        $buf .= $chunk;
    }
}

function message(int $opcode, string $data): array
{
    return [1, ['opcode' => $opcode, 'finish' => true, 'length' => strlen($data), 'md5' => md5($data)]];
}

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();
        $close = ws_frame(8, pack('n', 1000));
        $closed = [8, pack('n', 1000)];
        $big1 = random_bytes(40000);
        $big2 = random_bytes(30000);
        $cases = [
            'text in three fragments' => [
                ws_frame(1, 'hel', false) . ws_frame(0, 'lo ', false) . ws_frame(0, 'world'),
                [message(1, 'hello world')],
            ],
            'large binary in two fragments' => [
                ws_frame(2, $big1, false) . ws_frame(0, $big2),
                [message(2, $big1 . $big2)],
            ],
            'empty first fragment' => [
                ws_frame(1, '', false) . ws_frame(0, 'tail'),
                [message(1, 'tail')],
            ],
            'ping between fragments' => [
                ws_frame(1, 'abc', false) . ws_frame(9, 'p1') . ws_frame(0, 'def'),
                [[10, 'p1'], message(1, 'abcdef')],
            ],
            'pings between every fragment' => [
                ws_frame(2, 'a', false) . ws_frame(9, 'x') . ws_frame(0, 'b', false) . ws_frame(9, 'y') .
                    ws_frame(0, 'c'),
                [[10, 'x'], [10, 'y'], message(2, 'abc')],
            ],
            'pong between fragments' => [
                ws_frame(1, 'abc', false) . ws_frame(10, 'unsolicited') . ws_frame(0, 'def'),
                [message(1, 'abcdef')],
            ],
            'messages after a fragmented message' => [
                ws_frame(1, 'a', false) . ws_frame(0, 'b') . ws_frame(1, 'c') . ws_frame(2, 'd', false) .
                    ws_frame(0, 'e'),
                [message(1, 'ab'), message(1, 'c'), message(2, 'de')],
            ],
            // the close frame ends the connection, the unfinished message is dropped
            'close between fragments' => [
                ws_frame(1, 'abc', false) . ws_frame(8, pack('n', 1000) . 'bye'),
                [[8, pack('n', 1000) . 'bye']],
            ],
        ];
        foreach ($cases as $name => [$send, $expect]) {
            $cli = ws_open($port);
            Assert::true($cli->send($send) === strlen($send));
            $buf = '';
            $frames = [];
            foreach ($expect as $_) {
                $frames[] = ws_read_frame($cli, $buf);
            }
            if (end($expect)[0] !== 8) {
                // the close reply comes next, nothing else was sent
                $cli->send($close);
                $frames[] = ws_read_frame($cli, $buf);
                $expect[] = $closed;
            }
            $frames[] = ws_read_frame($cli, $buf);
            $expect[] = null;
            $cli->close();
            if ($frames !== $expect) {
                echo "$name: got " . var_export($frames, true) . "\n";
            }
        }
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::POOL_MODE);
    $serv->set(['worker_num' => 1, 'log_file' => '/dev/null']);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
        $serv->push($frame->fd, json_encode([
            'opcode' => $frame->opcode,
            'finish' => $frame->finish,
            'length' => strlen($frame->data),
            'md5' => md5($frame->data),
        ]));
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
