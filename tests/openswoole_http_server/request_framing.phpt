--TEST--
openswoole_http_server: Content-Length and Transfer-Encoding
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $request = function (array $parts) use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            foreach ($parts as $part) {
                $cli->send($part);
                co::usleep(50000);
            }
            $data = $cli->recv(3);
            if (!$data) {
                return 'closed';
            }
            $body = explode("\r\n\r\n", $data, 2)[1] ?? '';
            return strtok($data, "\r\n") . ($body !== '' ? " [{$body}]" : '');
        };
        $head = "POST / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n";
        echo 'gzip, chunked: ', $request([$head . "Transfer-Encoding: gzip, chunked\r\n\r\n4\r\nAAAA\r\n0\r\n\r\n"]), "\n";
        echo 'two lengths: ', $request([$head . "Content-Length: 4\r\nContent-Length: 10\r\n\r\nAAAA"]), "\n";
        echo 'chunked and length: ', $request([$head . "Transfer-Encoding: chunked\r\nContent-Length: 5\r\n\r\n0\r\n\r\n"]), "\n";
        echo 'chunked, gzip: ', $request([$head . "Transfer-Encoding: chunked, gzip\r\n\r\n0\r\n\r\n"]), "\n";
        echo 'split request line: ', $request(["GET / HTT", "P/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"]), "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null', 'worker_num' => 1]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end('len=' . strlen((string) $request->getContent()));
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
gzip, chunked: HTTP/1.1 200 OK [len=4]
two lengths: HTTP/1.1 400 Bad Request
chunked and length: HTTP/1.1 400 Bad Request
chunked, gzip: HTTP/1.1 400 Bad Request
split request line: HTTP/1.1 200 OK [len=0]
