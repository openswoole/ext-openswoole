--TEST--
openswoole_http_server: sendfile with dispatch_mode=7
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Http\Client;
use OpenSwoole\WebSocket\Server;

$pm = new ProcessManager;

$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $n = MAX_CONCURRENCY_LOW;
        while ($n--) {
            go(function () use ($pm) {
                $cli = new Client('127.0.0.1', $pm->getFreePort());
                $cli->setHeaders(['KeepAlive' => 'off', 'Connection' => 'close']);
                for ($i = MAX_REQUESTS_LOW; $i--;) {
                    $cli->get('/');
                    Assert::contains($cli->getBody(), 'openswoole_http_server: sendfile with dispatch_mode=7');
                }
            });
        }
    });
    echo "DONE\n";
    $pm->kill();
};

$pm->childFunc = function () use ($pm) {
    $http = new Server('0.0.0.0', $pm->getFreePort(), SWOOLE_PROCESS, SWOOLE_SOCK_TCP);
    $http->set([
        'log_file' => '/dev/null',
        'dispatch_mode' => 7,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) {
        $response->sendfile(__FILE__);
    });

    $http->on('message', function(){});
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
