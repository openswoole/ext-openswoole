--TEST--
swoole_client_coro: udp client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;
$pm->initFreePorts(2);

$pm->parentFunc = function ($pid) use ($pm)
{
    go(function () use ($pm) {
        echo httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        $pm->kill();
    });
};

$pm->childFunc = function () use ($pm)
{
    $http = new swoole_http_server('127.0.0.1', $pm->getFreePort(0), SWOOLE_BASE);

    $port2 = $http->listen('127.0.0.1', $pm->getFreePort(1), SWOOLE_SOCK_UDP);
    $port2->set([]);
    $port2->on('Packet', function ($serv, $data, $client)
    {
        $serv->sendto($client['address'], $client['port'], "Swoole: $data");
    });

    $http->set(array(
        'log_file' => '/dev/null'
    ));
    $http->on("WorkerStart", function (\swoole_server $serv)
    {
        /**
         * @var $pm ProcessManager
         */
        global $pm;
        $pm->wakeup();
    });
    $http->on('request', function (swoole_http_request $request, swoole_http_response $response) use ($pm)
    {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_UDP);
        if (!$cli->connect('127.0.0.1', $pm->getFreePort(1)))
        {
            fail:
            $response->end("ERROR\n");
            return;
        }
        if (!$cli->send("hello"))
        {
            goto fail;
        }
        $ret = $cli->recv();
        if (!$ret)
        {
            goto fail;
        }
        $response->end("OK\n");
    });
    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
OK
