--TEST--
openswoole_http2_server: a stream reset or disconnected while send_yield waits for the output buffer
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_http2();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

function frame(int $type, int $flags, int $stream, string $payload): string
{
    return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $stream) . $payload;
}

// requests $path on a new connection and leaves the response unread until the server output buffer is full
function start_big_download(int $port, string $path): OpenSwoole\Coroutine\Client
{
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    $cli->set(['socket_buffer_size' => 64 * 1024]);
    Assert::true($cli->connect('127.0.0.1', $port));
    // SETTINGS_INITIAL_WINDOW_SIZE = 2^31-1, the stream window does not limit the response
    $settings = frame(0x4, 0, 0, pack('nN', 0x4, 0x7fffffff));
    // END_STREAM | END_HEADERS; :method GET, :scheme http, :path as a literal
    $headers = frame(0x1, 0x5, 1, "\x82\x86\x04" . chr(strlen($path)) . $path);
    $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . $settings . $headers);
    co::usleep(500000);
    return $cli;
}

$pm = new SwooleTest\ProcessManager;
$reset = new OpenSwoole\Atomic(0);
$closed = new OpenSwoole\Atomic(0);
$pm->parentFunc = function () use ($pm, $reset, $closed) {
    co::run(function () use ($pm, $reset, $closed) {
        $port = $pm->getFreePort();
        $describe = function (OpenSwoole\Atomic $a) {
            return [0 => 'never', 1 => 'false', 2 => 'true'][$a->get()];
        };

        // RST_STREAM(CANCEL), then read everything so the server output buffer drains
        $cli = start_big_download($port, '/reset');
        $cli->send(frame(0x3, 0, 1, pack('N', 0x8)));
        $received = 0;
        while (($data = $cli->recv(1)) !== false && $data !== '') {
            $received += strlen($data);
        }
        Assert::lessThan($received, 32 * 1024 * 1024);
        co::usleep(100000);
        echo 'reset: end() returned ', $describe($reset), "\n";
        $cli->close();

        $cli = start_big_download($port, '/close');
        $cli->close();
        co::usleep(300000);
        echo 'close: end() returned ', $describe($closed), "\n";

        $h2 = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $port);
        Assert::true($h2->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/small';
        Assert::assert($h2->send($req));
        $resp = $h2->recv(3);
        echo $resp ? $resp->data : 'no response', "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm, $reset, $closed) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'open_http2_protocol' => true,
        'send_yield' => true,
        'socket_buffer_size' => 128 * 1024,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) use ($reset, $closed) {
        $uri = $request->server['request_uri'];
        if ($uri === '/small') {
            $response->end('ok');
            return;
        }
        $ret = $response->end(str_repeat('A', 32 * 1024 * 1024));
        ($uri === '/reset' ? $reset : $closed)->set($ret ? 2 : 1);
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
reset: end() returned false
close: end() returned false
ok
