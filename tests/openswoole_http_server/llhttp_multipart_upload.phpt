--TEST--
swoole_http_server: llhttp - multipart form data with file upload
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

        // Test 1: multipart with form fields and file upload
        $cli = new Client('127.0.0.1', $port);
        $cli->set(['timeout' => 5]);
        $cli->setData([
            'name' => 'test_user',
            'email' => 'test@example.com',
        ]);
        $cli->addFile(__FILE__, 'upload', 'text/x-php', 'test.php');
        Assert::true($cli->execute('/upload'));
        $result = json_decode($cli->body, true);
        Assert::same($result['post']['name'], 'test_user');
        Assert::same($result['post']['email'], 'test@example.com');
        Assert::same($result['files']['upload']['name'], 'test.php');
        Assert::same($result['files']['upload']['type'], 'text/x-php');
        Assert::greaterThan($result['files']['upload']['size'], 0);
        Assert::same($result['files']['upload']['error'], 0);

        // Test 2: multiple file uploads
        $cli2 = new Client('127.0.0.1', $port);
        $cli2->set(['timeout' => 5]);
        $cli2->setData(['description' => 'multi upload']);
        $cli2->addFile(__FILE__, 'file1', 'text/x-php', 'a.php');
        $cli2->addFile(__FILE__, 'file2', 'text/x-php', 'b.php');
        Assert::true($cli2->execute('/upload'));
        $result = json_decode($cli2->body, true);
        Assert::same($result['post']['description'], 'multi upload');
        Assert::same($result['files']['file1']['name'], 'a.php');
        Assert::same($result['files']['file2']['name'], 'b.php');

        // Test 3: upload with data from string
        $cli3 = new Client('127.0.0.1', $port);
        $cli3->set(['timeout' => 5]);
        $cli3->setMethod('POST');
        $fileContent = "This is test file content.\nLine 2.\n";
        $cli3->addData($fileContent, 'myfile', 'text/plain', 'hello.txt');
        Assert::true($cli3->execute('/upload'));
        $result = json_decode($cli3->body, true);
        Assert::same($result['files']['myfile']['name'], 'hello.txt');
        Assert::same($result['files']['myfile']['type'], 'text/plain');
        Assert::same($result['files']['myfile']['size'], strlen($fileContent));

        $cli->close();
        $cli2->close();
        $cli3->close();
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
            'post' => $request->post ?? [],
            'files' => [],
        ];
        if ($request->files) {
            foreach ($request->files as $name => $file) {
                $result['files'][$name] = [
                    'name' => $file['name'],
                    'type' => $file['type'],
                    'size' => $file['size'],
                    'error' => $file['error'],
                ];
            }
        }
        $response->end(json_encode($result));
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
SUCCESS
