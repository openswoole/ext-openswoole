--TEST--
openswoole_websocket_server: invalid frames close the connection and the server keeps serving
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new SwooleTest\ProcessManager;

const MAX_LENGTH = 65536;

function ws_frame(int $opcode, string $payload, bool $fin = true, bool $masked = true): string
{
    $len = strlen($payload);
    $head = chr(($fin ? 0x80 : 0) | $opcode);
    $m = $masked ? 0x80 : 0;
    if ($len < 126) {
        $head .= chr($m | $len);
    } elseif ($len < 65536) {
        $head .= chr($m | 126) . pack('n', $len);
    } else {
        $head .= chr($m | 127) . pack('J', $len);
    }
    if (!$masked) {
        return $head . $payload;
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

// everything the server sends until it closes the connection
function ws_read_until_closed(OpenSwoole\Coroutine\Client $cli): string
{
    $data = '';
    while (true) {
        $chunk = $cli->recv(5);
        if ($chunk === '') {
            break;
        }
        if ($chunk === false) {
            Assert::notSame($cli->errCode, SOCKET_ETIMEDOUT);
            break;
        }
        $data .= $chunk;
    }
    $cli->close();
    return $data;
}

function ws_echo_check(int $port): void
{
    $cli = ws_open($port);
    $cli->send(ws_frame(1, 'still serving'));
    $cli->send(ws_frame(8, pack('n', 1000)));
    Assert::same(bin2hex(ws_read_until_closed($cli)), bin2hex("\x81\x0dstill serving\x88\x02\x03\xe8"));
}

$pm->parentFunc = function (int $pid) use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();
        $close1000 = ws_frame(8, pack('n', 1000));
        $cases = [
            // the frame header announces a payload over package_max_length
            'frame over package_max_length' => ["\x81" . chr(0x80 | 127) . pack('J', MAX_LENGTH + 1) . random_bytes(4), ''],
            'continuation without a start frame' => [ws_frame(0, 'abc'), ''],
            'fragments over package_max_length' => [
                ws_frame(1, str_repeat('a', 40000), false) . ws_frame(0, str_repeat('b', 40000)),
                '',
            ],
            'new fragmented message inside a fragmented message' => [ws_frame(1, 'a', false) . ws_frame(1, 'b', false), ''],
            'new message inside a fragmented message' => [ws_frame(1, 'a', false) . ws_frame(1, 'b'), ''],
            'ping over 125 bytes' => [ws_frame(9, str_repeat('p', 126)), ''],
            'pong over 125 bytes' => [ws_frame(10, str_repeat('p', 126)), ''],
            'close reason too long' => [ws_frame(8, pack('n', 1000) . str_repeat('r', 124)), ''],
            'unknown data opcode' => [ws_frame(3, 'x'), ''],
            'unknown control opcode' => [ws_frame(11, 'x'), ''],
            'unmasked frame' => [ws_frame(1, 'hello', true, false), ''],
            // the largest control frames are accepted
            'ping of 125 bytes' => [
                ws_frame(9, str_repeat('p', 125)) . $close1000,
                "\x8a\x7d" . str_repeat('p', 125) . "\x88\x02\x03\xe8",
            ],
            'close of 125 bytes' => [
                ws_frame(8, pack('n', 1000) . str_repeat('r', 123)),
                "\x88\x7d" . pack('n', 1000) . str_repeat('r', 123),
            ],
        ];
        foreach ($cases as $name => [$send, $expect]) {
            $cli = ws_open($port);
            Assert::true($cli->send($send) === strlen($send));
            $data = ws_read_until_closed($cli);
            if ($data !== $expect) {
                echo "$name: got " . bin2hex(substr($data, 0, 64)) . "\n";
            }
            ws_echo_check($port);
        }
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $serv = new OpenSwoole\WebSocket\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::SIMPLE_MODE);
    $serv->set(['worker_num' => 1, 'log_file' => '/dev/null', 'package_max_length' => MAX_LENGTH]);
    $serv->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $serv->on('Message', function (OpenSwoole\WebSocket\Server $serv, OpenSwoole\WebSocket\Frame $frame) {
        $serv->push($frame->fd, $frame->data);
    });
    $serv->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
