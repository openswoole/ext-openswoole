--TEST--
openswoole_http2_server: frames on a stream the client has ended
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
        return 'GOAWAY ' . unpack('N', substr($f[3], 4, 4))[1];
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

// GET / and GET /b from the static table
const GET_SLOW = "\x82\x86\x84";
const GET_FAST = "\x82\x86\x04\x02/b";

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $port = $pm->getFreePort();

        // DATA after END_STREAM while the response is pending
        $cli = connect($port);
        $buffer = '';
        $cli->send(frame(1, 0x5, 1, GET_SLOW));
        co::usleep(100000);
        $cli->send(frame(0, 0x1, 1, 'X'));
        echo 'data: ', next_frame($cli, $buffer), "\n";
        $cli->send(frame(1, 0x5, 3, GET_FAST));
        echo 'next: ', next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), "\n";

        // DATA on a stream whose response has ended
        $cli->send(frame(0, 0x1, 3, 'X'));
        echo 'ended: ', next_frame($cli, $buffer), "\n";
        $cli->close();

        // HEADERS after END_STREAM
        $cli = connect($port);
        $buffer = '';
        $cli->send(frame(1, 0x5, 1, GET_SLOW));
        co::usleep(100000);
        $cli->send(frame(1, 0x5, 1, GET_SLOW));
        echo 'headers: ', next_frame($cli, $buffer), "\n";

        // the worker still serves requests
        $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $port);
        Assert::true($cli->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/b';
        Assert::assert($cli->send($req));
        echo 'after: ', $cli->recv(3)->data, "\n";
        echo 'requests: ', file_get_contents('http://127.0.0.1:' . $port . '/count'), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $count = 0;
    $http->on('request', function ($request, $response) use (&$count) {
        $count++;
        $path = $request->server['request_uri'];
        if ($path === '/count') {
            // the worker that served the first request is still alive
            $response->end((string) $count);
        } elseif ($path === '/b') {
            $response->end('b');
        } else {
            // only $response outlives the handler
            go(function () use ($response) {
                co::usleep(300000);
                if ($response->isWritable()) {
                    $response->end('late');
                }
            });
        }
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
data: RST_STREAM#1 5
next: HEADERS#3 DATA#3 b
ended: RST_STREAM#3 5
headers: closed
after: b
requests: 5
