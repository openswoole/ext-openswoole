--TEST--
openswoole_http_client_coro: http client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    go(function () use ($pm) {
        echo httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        $pm->kill();
    });
};
$pm->childFunc = function () use ($pm) {
    // started here so that it is a child of this server, not of the ProcessManager parent
    $server = SwooleTest\LocalServer::start();
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null'
    ]);
    $http->on('WorkerStart', function (\openswoole_server $serv) {
        /**
         * @var $pm ProcessManager
         */
        global $pm;
        $pm->wakeup();
    });
    $http->on('WorkerStop', function () use ($server) {
        $server->stop();
    });
    $http->on('request', function (openswoole_http_request $request, openswoole_http_response $response) use ($server) {
        $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpsPort, true);
        $cli->set(['timeout' => 10]);
        $cli->setHeaders([
            'Host' => $server->host,
            'User-Agent' => 'Chrome/49.0.2587.3',
            'Accept' => 'text/html,application/xhtml+xml,application/xml',
            'Accept-Encoding' => 'gzip',
        ]);
        $ret = ($cli->get('/'));
        if (!$ret) {
            $response->end("ERROR\n");
            return;
        } else {
            $response->end("OK\n");
            $cli->close();
        }
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
OK
