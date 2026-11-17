--TEST--
openswoole_http2_server: header blocks in CONTINUATION frames and a larger header table
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function frame(int $type, int $flags, int $streamId, string $payload): string
{
    return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $streamId) . $payload;
}

// :method GET, :scheme http, :path / from the static table, :authority and x-test as literals
function header_block(string $value): string
{
    return "\x82\x86\x84" . "\x01\x09localhost" . "\x00\x06x-test" . chr(strlen($value)) . $value;
}

// sends one request and returns ":status body"
function request(int $port, array $frames): string
{
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect('127.0.0.1', $port, 5));
    $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . frame(4, 0, 0, ''));
    foreach ($frames as $f) {
        $cli->send($f);
    }
    $buffer = '';
    $status = '?';
    $body = '';
    $deadline = microtime(true) + 5;
    while (microtime(true) < $deadline) {
        $data = $cli->recv(1);
        if ($data === '' || $data === false) {
            if ($data === '') {
                break;
            }
            continue;
        }
        $buffer .= $data;
        while (strlen($buffer) >= 9) {
            $length = unpack('N', "\0" . substr($buffer, 0, 3))[1];
            if (strlen($buffer) < 9 + $length) {
                break;
            }
            $type = ord($buffer[3]);
            $flags = ord($buffer[4]);
            $payload = substr($buffer, 9, $length);
            $buffer = substr($buffer, 9 + $length);
            if ($type === 1) {
                // the response starts with :status from the static table: 0x88 is 200
                $status = $payload[0] === "\x88" ? '200' : bin2hex($payload[0]);
            } elseif ($type === 0) {
                $body .= $payload;
            } elseif ($type === 7) {
                return "goaway";
            }
            if (($type === 0 || $type === 1) && ($flags & 0x1)) {
                return "{$status} {$body}";
            }
        }
    }
    return "{$status} {$body}";
}

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();
        // HEADERS without END_HEADERS, then two CONTINUATION frames
        $block = header_block('split');
        echo 'continuation: ', request($port, [
            frame(1, 0x01, 1, substr($block, 0, 5)),
            frame(9, 0x00, 1, substr($block, 5, 10)),
            frame(9, 0x04, 1, substr($block, 15)),
        ]), "\n";
        // a dynamic table size update to the advertised 8192 bytes
        echo 'table size: ', request($port, [frame(1, 0x05, 1, "\x3f\xe1\x3f" . header_block('table'))]), "\n";
        // a frame of another stream inside a header block is a protocol error
        echo 'interleaved: ', request($port, [
            frame(1, 0x01, 1, substr($block, 0, 5)),
            frame(1, 0x05, 3, $block),
        ]), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'open_http2_protocol' => true,
        'http2_header_table_size' => 8192,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end($request->header['x-test'] ?? 'missing');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
continuation: 200 split
table size: 200 table
interleaved: ? 
