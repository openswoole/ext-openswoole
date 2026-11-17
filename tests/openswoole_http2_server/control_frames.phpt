--TEST--
openswoole_http2_server: control frames with a wrong length close the connection
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
        $frame = function (int $type, int $flags, int $stream, string $payload) {
            return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $stream) . $payload;
        };
        foreach ([
            'ping' => $frame(0x6, 0, 0, ''),
            'window_update' => $frame(0x8, 0, 0, "\0\0"),
            'settings' => $frame(0x4, 0, 0, "\0\x03\0"),
            'settings ack' => $frame(0x4, 0x1, 0, "\0\x03\0\0\0\x01"),
        ] as $name => $bad) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . $frame(0x4, 0, 0, '') . $bad);
            $closed = false;
            while (($data = $cli->recv(3)) !== false) {
                if ($data === '') {
                    $closed = true;
                    break;
                }
            }
            echo $name, ': ', $closed || $cli->errCode !== SOCKET_ETIMEDOUT ? 'closed' : 'open', "\n";
        }

        $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($cli->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/';
        Assert::assert($cli->send($req));
        echo $cli->recv(3)->data, "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end('ok');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
ping: closed
window_update: closed
settings: closed
settings ack: closed
ok
