--TEST--
openswoole_http_server: chunked with Content-Length 0 must end with the chunked terminator
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $request = function (string $data) use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $cli->send($data);
            $out = '';
            while (($chunk = $cli->recv(1)) !== '' && $chunk !== false) {
                $out .= $chunk;
            }
            preg_match_all('#^HTTP/1\.1 (\d+)#m', $out, $m);
            return $m[1] ? implode(' ', $m[1]) : 'closed';
        };
        $head = "POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nContent-Length: 0\r\n\r\n";
        $next = "GET /second HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
        echo 'bad terminator: ', $request($head . "XXXXX" . $next), "\n";
        // headers that end exactly at the end of the 64 KiB receive buffer
        $lines = "POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\nContent-Length: 0\r\n";
        $pad = 65536 - strlen($lines) - strlen("X-Pad: \r\n\r\n");
        $full = $lines . 'X-Pad: ' . str_repeat('a', $pad) . "\r\n\r\n";
        Assert::same(strlen($full), 65536);
        echo 'full buffer: ', $request($full . "0\r\n\r\n"), "\n";
        co::usleep(100000);
        echo 'count: ', $request("GET /count HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_PROCESS);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $count = 0;
    $http->on('request', function ($request, $response) use (&$count) {
        if ($request->server['request_uri'] === '/count') {
            $response->status(200 + $count);
            $response->end();
            return;
        }
        $count++;
        $response->end('ok');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
bad terminator: 400
full buffer: 400
count: 200
