--TEST--
openswoole_http2_server: multipart body in several DATA frames
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

// POST / with a content-type literal of the static table name
const CONTENT_TYPE = 'multipart/form-data; boundary=XyZ';
const POST = "\x83\x86\x84\x0f\x10" . "\x21" . CONTENT_TYPE;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = connect($pm->getFreePort());
        $buffer = '';
        $body = "--XyZ\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nhello\r\n"
            . "--XyZ\r\nContent-Disposition: form-data; name=\"b\"\r\n\r\nworld\r\n--XyZ--\r\n";
        $cli->send(frame(1, 0x4, 1, POST));
        foreach (str_split($body, 20) as $i => $chunk) {
            $cli->send(frame(0, 0, 1, $chunk));
        }
        $cli->send(frame(0, 0x1, 1, ''));
        echo next_frame($cli, $buffer), ' ', next_frame($cli, $buffer), "\n";
        $cli->close();
    });
    $pm->kill();
    echo strpos(file_get_contents(TEST_LOG_FILE), 'parse multipart body failed') === false ? 'no warning' : 'warning', "\n";
    @unlink(TEST_LOG_FILE);
};
$pm->childFunc = function () use ($pm) {
    @unlink(TEST_LOG_FILE);
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => TEST_LOG_FILE, 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end(json_encode($request->post));
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
HEADERS#1 DATA#1 {"a":"hello","b":"world"}
no warning
