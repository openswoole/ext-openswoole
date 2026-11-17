--TEST--
openswoole_http_server: chunk size
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $request = function (string $body) use ($pm) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $cli->send("POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n" . $body);
            $data = $cli->recv(3);
            return $data ? strtok($data, "\r\n") : 'closed';
        };
        // the end pointer wraps around to the start of the chunk
        echo $request("FFFFFFFFFFFFFFEC\r\nAAAA\r\n"), "\n";
        // 2^32 truncates to 0 in a 32-bit length
        echo $request("100000000\r\nAAAA\r\n"), "\n";
        // not a hex digit
        echo $request("1G\r\nAAAA\r\n"), "\n";
        echo $request("4\r\nAAAA\r\n0\r\n\r\n"), "\n";
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
        $response->end($request->getContent());
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
HTTP/1.1 400 Bad Request
HTTP/1.1 400 Bad Request
HTTP/1.1 400 Bad Request
HTTP/1.1 200 OK
