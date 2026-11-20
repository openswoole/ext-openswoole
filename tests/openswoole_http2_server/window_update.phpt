--TEST--
openswoole_http2_server: WINDOW_UPDATE with a zero increment or a window above 2^31-1
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function frame(int $type, int $flags, int $streamId, string $payload): string
{
    return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $streamId) . $payload;
}

// returns [type, flags, stream, payload], 'closed' or 'timeout'
function read_frame(OpenSwoole\Coroutine\Client $cli, string &$buffer, float $timeout = 3)
{
    $deadline = microtime(true) + $timeout;
    while (true) {
        if (strlen($buffer) >= 9) {
            $length = unpack('N', "\0" . substr($buffer, 0, 3))[1];
            if (strlen($buffer) >= 9 + $length) {
                $frame = [ord($buffer[3]), ord($buffer[4]), unpack('N', substr($buffer, 5, 4))[1] & 0x7fffffff, substr($buffer, 9, $length)];
                $buffer = substr($buffer, 9 + $length);
                return $frame;
            }
        }
        if (microtime(true) >= $deadline) {
            return 'timeout';
        }
        $data = $cli->recv(0.5);
        if ($data === '') {
            return 'closed';
        }
        if ($data !== false) {
            $buffer .= $data;
        }
    }
}

// the next frame other than SETTINGS and WINDOW_UPDATE
function next_frame(OpenSwoole\Coroutine\Client $cli, string &$buffer): string
{
    while (is_array($f = read_frame($cli, $buffer)) && ($f[0] === 4 || $f[0] === 8));
    if (!is_array($f)) {
        return $f;
    }
    switch ($f[0]) {
    case 0:
        return "DATA#{$f[2]} {$f[3]}";
    case 1:
        return "HEADERS#{$f[2]}";
    case 3:
        return "RST_STREAM#{$f[2]} " . unpack('N', $f[3])[1];
    case 7:
        return "GOAWAY {$f[1]} " . unpack('N', substr($f[3], 4, 4))[1];
    default:
        return "type {$f[0]}";
    }
}

function connect(int $port): OpenSwoole\Coroutine\Client
{
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect('127.0.0.1', $port, 5));
    $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . frame(4, 0, 0, ''));
    return $cli;
}

function window_update(int $streamId, int $increment): string
{
    return frame(8, 0, $streamId, pack('N', $increment));
}

// GET / with a body to follow, GET /b without one
const GET_OPEN = "\x82\x86\x84";
const GET_FAST = "\x82\x86\x04\x02/b";

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();

        // connection errors
        foreach (['zero' => 0, 'above' => 0x7fffffff - 65535 + 1] as $name => $increment) {
            $cli = connect($port);
            $buffer = '';
            $cli->send(window_update(0, $increment));
            echo "connection {$name}: ", next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), "\n";
            $cli->close();
        }

        // stream errors keep the connection
        $cli = connect($port);
        $buffer = '';
        $cli->send(window_update(0, 0x7fffffff - 65535));
        $cli->send(frame(1, 0x4, 1, GET_OPEN));
        $cli->send(window_update(1, 0));
        echo 'stream zero: ', next_frame($cli, $buffer), "\n";
        $cli->send(frame(1, 0x4, 3, GET_OPEN));
        $cli->send(window_update(3, 65535));
        $cli->send(window_update(3, 0x7fffffff - 2 * 65535 + 1));
        echo 'stream above: ', next_frame($cli, $buffer), "\n";
        // a stream that is gone and the reserved bit
        $cli->send(window_update(1, 100));
        $cli->send(frame(1, 0x4, 5, GET_OPEN));
        $cli->send(window_update(5, 0x80000000 | (0x7fffffff - 65535)));
        $cli->send(frame(0, 0x1, 5, ''));
        $cli->send(frame(1, 0x5, 7, GET_FAST));
        echo 'requests: ', next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), "\n";
        $cli->close();
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end($request->server['request_uri']);
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
connection zero: GOAWAY 0 1 closed
connection above: GOAWAY 0 3 closed
stream zero: RST_STREAM#1 1
stream above: RST_STREAM#3 3
requests: HEADERS#5 DATA#5 / HEADERS#7 DATA#7 /b
