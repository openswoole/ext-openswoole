--TEST--
openswoole_http_server: llhttp - POST body with Content-Length and form-urlencoded
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;
use OpenSwoole\Coroutine\Http\Client;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    Coroutine::run(function () use ($pm) {
        $port = $pm->getFreePort();

        // Test 1: POST with application/x-www-form-urlencoded
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setMethod('POST');
        $cli->setData('username=admin&password=secret&remember=1');
        Assert::true($cli->execute('/post'));
        $result = json_decode($cli->body, true);
        Assert::same($result['post']['username'], 'admin');
        Assert::same($result['post']['password'], 'secret');
        Assert::same($result['post']['remember'], '1');
        Assert::same($result['method'], 'POST');

        // Test 2: POST with raw JSON body
        $cli->setHeaders(['Content-Type' => 'application/json']);
        $cli->setMethod('POST');
        $jsonBody = json_encode(['key' => 'value', 'list' => [1, 2, 3]]);
        $cli->setData($jsonBody);
        Assert::true($cli->execute('/json'));
        $result = json_decode($cli->body, true);
        Assert::same($result['raw_content'], $jsonBody);
        Assert::same($result['content_type'], 'application/json');

        // Test 3: PUT with body
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setMethod('PUT');
        $cli->setData('field1=updated&field2=data');
        Assert::true($cli->execute('/put'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'PUT');
        Assert::same($result['post']['field1'], 'updated');

        // Test 4: DELETE with body
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setMethod('DELETE');
        $cli->setData('id=42');
        Assert::true($cli->execute('/delete'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'DELETE');
        Assert::same($result['post']['id'], '42');

        // Test 5: POST with empty body
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setMethod('POST');
        $cli->setData('');
        Assert::true($cli->execute('/empty'));
        $result = json_decode($cli->body, true);
        Assert::same($result['raw_len'], 0);

        // Test 6: PATCH with body
        $cli->setHeaders(['Content-Type' => 'application/x-www-form-urlencoded']);
        $cli->setMethod('PATCH');
        $cli->setData('name=patched');
        Assert::true($cli->execute('/patch'));
        $result = json_decode($cli->body, true);
        Assert::same($result['method'], 'PATCH');
        Assert::same($result['post']['name'], 'patched');

        $cli->close();
    });
    echo "SUCCESS\n";
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'http_compression' => false,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
        $result = [
            'method' => $request->server['request_method'],
            'post' => $request->post ?? [],
            'raw_content' => $request->rawContent(),
            'raw_len' => strlen($request->rawContent()),
            'content_type' => $request->header['content-type'] ?? '',
        ];
        $response->end(json_encode($result));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
