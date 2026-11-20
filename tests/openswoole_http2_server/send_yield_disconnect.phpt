--TEST--
openswoole_http2_server: a send_yield response ends when the client disconnects while the window is open
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
$result = new OpenSwoole\Atomic(0);
$pm->parentFunc = function () use ($pm, $result) {
    co::run(function () use ($pm, $result) {
        $frame = function (int $type, int $flags, int $stream, string $payload) {
            return substr(pack('N', strlen($payload)), 1) . chr($type) . chr($flags) . pack('N', $stream) . $payload;
        };
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        // END_STREAM | END_HEADERS; :method GET, :path /, :scheme http; open both windows wide
        $cli->send("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n" . $frame(0x4, 0, 0, '') . $frame(0x1, 0x5, 1, "\x82\x84\x86")
            . $frame(0x8, 0, 0, pack('N', 0x7fff0000)) . $frame(0x8, 0, 1, pack('N', 0x7fff0000)));
        Assert::notEmpty($cli->recv(3));
        $cli->close();

        for ($i = 0; $i < 50 && $result->get() === 0; $i++) {
            co::usleep(100000);
        }
        echo 'end() returned: ', $result->get() == 1 ? 'false' : ($result->get() == 2 ? 'true' : 'never'), "\n";

        $h2 = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        $h2->set(['timeout' => 3]);
        Assert::true($h2->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/small';
        Assert::assert($h2->send($req));
        echo $h2->recv(3)->data, "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm, $result) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), Server::SIMPLE_MODE);
    $http->set([
        'log_file' => '/dev/null',
        'worker_num' => 1,
        'open_http2_protocol' => true,
        'send_yield' => true,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) use ($result) {
        if ($request->server['request_uri'] === '/small') {
            $response->end('ok');
            return;
        }
        $ret = $response->end(str_repeat('A', 32 * 1024 * 1024));
        $result->set($ret ? 2 : 1);
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
end() returned: false
ok
