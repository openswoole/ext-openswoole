--TEST--
openswoole_http_server: form field of a multipart body without closing boundary is not kept for the next request
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function multipart_request(string $body): string
{
    return "POST / HTTP/1.1\r\n" .
        "Host: 127.0.0.1\r\n" .
        "Connection: close\r\n" .
        "Content-Type: multipart/form-data; boundary=XX\r\n" .
        "Content-Length: " . strlen($body) . "\r\n" .
        "\r\n" .
        $body;
}

function send_request(int $port, string $data): string
{
    $sock = stream_socket_client("tcp://127.0.0.1:{$port}", $errno, $errstr, 5);
    Assert::assert($sock !== false);
    fwrite($sock, $data);
    $response = stream_get_contents($sock);
    fclose($sock);
    [, $body] = explode("\r\n\r\n", $response, 2);
    return $body;
}

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    $port = $pm->getFreePort();
    // the last field has no closing boundary
    $body = send_request($port, multipart_request("--XX\r\nContent-Disposition: form-data; name=\"a\"\r\n\r\nFIRSTVALUE"));
    Assert::same(json_decode($body, true), []);

    $body = send_request($port, multipart_request("--XX\r\nContent-Disposition: form-data; name=\"b\"\r\n\r\nsecond\r\n--XX--\r\n"));
    Assert::same(json_decode($body, true), ['b' => 'second']);

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
        $response->end(json_encode($request->post ?? []));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
