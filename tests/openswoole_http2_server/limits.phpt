--TEST--
openswoole_http2_server: request body and concurrent streams are limited
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_http2();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        foreach ([32 * 1024, 256 * 1024] as $size) {
            $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
            Assert::true($cli->connect());
            $req = new OpenSwoole\Http2\Request;
            $req->method = 'POST';
            $req->path = '/';
            $req->data = str_repeat('A', $size);
            $cli->send($req);
            $res = $cli->recv(3);
            echo "body {$size}: ", $res ? $res->data : 'rejected', "\n";
        }

        // open more streams than SETTINGS_MAX_CONCURRENT_STREAMS without ending them
        $frame = function (int $type, int $flags, int $stream, string $payload) {
            return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $stream) . $payload;
        };
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $data = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . $frame(0x4, 0, 0, '');
        foreach ([1, 3, 5] as $stream) {
            // END_HEADERS only; :method GET, :path /, :scheme http from the static table
            $data .= $frame(0x1, 0x4, $stream, "\x82\x84\x86");
        }
        $cli->send($data);
        $closed = false;
        while (($recv = $cli->recv(3)) !== false) {
            if ($recv === '') {
                $closed = true;
                break;
            }
        }
        echo 'streams: ', $closed || $cli->errCode !== SOCKET_ETIMEDOUT ? 'closed' : 'open', "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'open_http2_protocol' => true,
        'package_max_length' => 128 * 1024,
        'http2_max_concurrent_streams' => 2,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end(strlen($request->getContent()));
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
body 32768: 32768
body 262144: rejected
streams: closed
