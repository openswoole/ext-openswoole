--TEST--
openswoole_http_server: upload part of a multipart body without closing boundary does not keep its file open
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_darwin();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const N = 20;

function send_request(int $port, string $body): string
{
    $sock = stream_socket_client("tcp://127.0.0.1:{$port}", $errno, $errstr, 5);
    Assert::assert($sock !== false);
    fwrite($sock, "POST / HTTP/1.1\r\n" .
        "Host: 127.0.0.1\r\n" .
        "Connection: close\r\n" .
        "Content-Type: multipart/form-data; boundary=XX\r\n" .
        "Content-Length: " . strlen($body) . "\r\n" .
        "\r\n" .
        $body);
    $response = stream_get_contents($sock);
    fclose($sock);
    [, $body] = explode("\r\n\r\n", $response, 2);
    return $body;
}

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    $port = $pm->getFreePort();
    // the file part has no closing boundary
    $truncated = "--XX\r\nContent-Disposition: form-data; name=\"f\"; filename=\"a.txt\"\r\n\r\nFILEDATA";
    $before = (int) send_request($port, $truncated);
    for ($i = 0; $i < N; $i++) {
        send_request($port, $truncated);
    }
    $after = (int) send_request($port, $truncated);
    Assert::same($after, $before);
    $pm->kill();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), OpenSwoole\Server::SIMPLE_MODE);
    $http->set(['worker_num' => 1, 'log_file' => '/dev/null']);
    $http->on('WorkerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('Request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $response->end((string) count(scandir('/proc/self/fd')));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
