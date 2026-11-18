--TEST--
openswoole_http2_server: HEADERS frame fields
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
$starts = new OpenSwoole\Atomic(0);
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $frame = function (int $type, int $flags, int $stream, string $payload) {
            return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $stream) . $payload;
        };
        foreach ([
            'priority' => $frame(0x1, 0x25, 1, ''),        // END_STREAM | END_HEADERS | PRIORITY, no payload
            'padded' => $frame(0x1, 0x0d, 1, "\xff"),      // END_STREAM | END_HEADERS | PADDED, pad length 255
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
$pm->childFunc = function () use ($pm, $starts) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm, $starts) {
        if ($starts->add(1) == 1) {
            $pm->wakeup();
        }
    });
    $http->on('request', function ($request, $response) use ($starts) {
        $response->end("worker_starts={$starts->get()}");
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
priority: closed
padded: closed
worker_starts=1
