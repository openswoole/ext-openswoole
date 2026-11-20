--TEST--
openswoole_http_server: QUERY method
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    $body = 'name=example';
    $request = implode("\r\n", [
        'QUERY /search?page=2 HTTP/1.1',
        'Host: 127.0.0.1',
        'Content-Type: application/x-www-form-urlencoded',
        'Content-Length: ' . strlen($body),
        'Connection: close',
        '',
        $body,
    ]);

    $client = stream_socket_client("tcp://127.0.0.1:{$pm->getFreePort()}");
    fwrite($client, $request);
    $response = stream_get_contents($client);
    fclose($client);
    [$header, $body] = explode("\r\n\r\n", $response, 2);

    Assert::contains($header, 'HTTP/1.1 200 OK');
    $data = json_decode($body, true);
    Assert::same($data['method'], 'QUERY');
    Assert::same($data['get'], ['page' => '2']);
    Assert::same($data['post'], ['name' => 'example']);
    Assert::same($data['content'], 'name=example');

    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set(['log_file' => '/dev/null']);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $response->end(json_encode([
            'method' => $request->server['request_method'],
            'get' => $request->get,
            'post' => $request->post,
            'content' => $request->getContent(),
        ]));
    });
    $server->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
