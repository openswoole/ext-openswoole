--TEST--
openswoole_http2_server: a response header block larger than the header buffer is refused
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
$pm->parentFunc = function () use ($pm, $starts) {
    co::run(function () use ($pm) {
        $h2 = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        $h2->set(['timeout' => 3]);
        Assert::true($h2->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/large';
        Assert::assert($h2->send($req));
        $res = $h2->recv(3);
        Assert::true($res === false || $res->statusCode !== 200);

        $h2 = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($h2->connect());
        $req = new OpenSwoole\Http2\Request;
        $req->path = '/small';
        Assert::assert($h2->send($req));
        Assert::same($h2->recv(3)->data, 'ok');
    });
    Assert::same($starts->get(), 1);
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm, $starts) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), Server::SIMPLE_MODE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1, 'open_http2_protocol' => true]);
    $http->on('workerStart', function () use ($pm, $starts) {
        $starts->add(1);
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        if ($request->server['request_uri'] === '/large') {
            $response->header('x-large', bin2hex(random_bytes(16 * 1024)));
            $response->end('large');
            return;
        }
        $response->end('ok');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
