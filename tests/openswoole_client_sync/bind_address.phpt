--TEST--
openswoole_client_sync: bind address
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();

use OpenSwoole\Client;

$pm = new ProcessManager;
$pm->parentFunc = function () use ($port, $pm) {
    $client = new Client(SWOOLE_SOCK_TCP);
    $client->set([
        'bind_address' => '127.0.0.1',
        'bind_port' => $pm->getFreePort(),
    ]);

    Assert::false($client->connect('127.0.0.1', $port));
    Assert::eq($client->errCode, SOCKET_EADDRINUSE);
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($port, $pm) {
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['worker_num' => 1, 'log_file' => '/dev/null']);
    $http->on('workerStart', function () use ($port, $pm) {
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($port) {
        $response->end('OK');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
Warning: OpenSwoole\Client::connect(): bind address or port error in set method in %s
DONE
