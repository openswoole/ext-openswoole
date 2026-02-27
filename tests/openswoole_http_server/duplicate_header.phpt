--TEST--
openswoole_http_server: duplicate header
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://127.0.0.1:{$pm->getFreePort()}/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, TRUE);
    echo curl_exec($ch);
    curl_close($ch);
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'worker_num' => 1,
        'enable_coroutine' => false,
        'enable_server_token' => true,
        'log_file' => '/dev/null'
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) {
        $msg = "hello world";
        $response->header("content-length", strlen($msg) . " ");
        $response->header("Test-Value", [
            "a\r\n",
            "b1234 ",
            "d5678",
            "e  \n ",
            null,
            5678,
            3.1415926,
        ]);
        $response->end($msg);
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECTF--
HTTP/1.1 200 OK
Content-Length: 11
Test-Value: a
Test-Value: b1234
Test-Value: d5678
Test-Value: e
Test-Value: 5678
Test-Value: 3.1415926
Server: OpenSwoole %s
Connection: keep-alive
Content-Type: text/html
Date: %s

hello world
