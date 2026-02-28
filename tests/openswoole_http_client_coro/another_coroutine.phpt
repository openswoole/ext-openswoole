--TEST--
openswoole_http_client_coro: illegal another coroutine
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_unsupported();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function (int $pid) use ($pm) {
    $process = new openswoole_process(function (openswoole_process $worker) use ($pm) {
        function get(OpenSwoole\Coroutine\Http\Client $client)
        {
            $client->get('/');
        }

        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        go(function () use ($cli) {
            (function () use ($cli) {
                (function () use ($cli) {
                    Co::usleep(1000);
                    get($cli);
                })();
            })();
        });
        go(function () use ($cli) {
            $cli->get('/');
        });
        openswoole_event_wait();
    }, false);
    $process->start();
    openswoole_process::wait();
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SERVER_MODE_RANDOM);
    $server->set(['log_file' => '/dev/null']);
    $server->on('workerStart', function (openswoole_http_server $server) use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($pm, $server) {
        co::usleep(100000);
        $server->shutdown();
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTF--
[%s]	ERROR	(PHP Fatal Error: %d):
OpenSwoole\Coroutine\Http\Client::get: Socket#%d has already been bound to another coroutine#%d, reading of the same socket in multiple coroutines at the same time is not allowed
Stack trace:
#0  OpenSwoole\Coroutine\Http\Client->get() called at [%s:%d]
#1  get() called at [%s:%d]
#2  {closure}() called at [%s:%d]
#3  {closure}() called at [%s:%d]
