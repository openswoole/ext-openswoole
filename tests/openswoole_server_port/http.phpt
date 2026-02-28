--TEST--
openswoole_server_port: http and tcp
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

ini_set("openswoole.display_errors", "Off");

$pm = new ProcessManager;
$pm->initFreePorts(2);

$pm->parentFunc = function ($pid) use ($pm)
{
    go(function () use ($pm)
    {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        $cli->set(['open_eof_check' => true, "package_eof" => "\r\n\r\n"]);
        if (!$cli->connect('127.0.0.1', $pm->getFreePort(0), 0.5))
        {
            fail:
            echo "ERROR 1\n";
            return;
        }
        //no eof, should be timeout here
        if (!$cli->send("hello\r\n\r\n"))
        {
            goto fail;
        }
        $ret = $cli->recv();
        if (!$ret)
        {
            goto fail;
        }
        echo "OK\n";
    });

    go(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort(1));
        if ($cli->get("/")) {
            echo $cli->body;
            Assert::same($cli->statusCode, 200);
        } else {
            echo "ERROR 2\n";
        }
    });

    openswoole_event_wait();
    openswoole_process::kill($pid);
};

$pm->childFunc = function () use ($pm)
{
    $server = new openswoole_server('127.0.0.1', $pm->getFreePort(0), SWOOLE_BASE);

    $server->set([
        'open_eof_check' => true,
        "package_eof"    => "\r\n\r\n",
        'log_file'       => '/dev/null'
    ]);

    $server->on('Receive', function ($serv, $fd, $rid, $data)
    {
        $serv->send($fd, "Swoole: $data\r\n\r\n");
    });

    $port2 = $server->listen('127.0.0.1', $pm->getFreePort(1), SWOOLE_SOCK_TCP);
    $port2->set(['open_http_protocol' => true,]);
    $port2->on("request", function ($req, $resp) {
        $resp->end("hello swooler\n");
    });

    $server->on("WorkerStart", function (\openswoole_server $serv) {
        /**
         * @var $pm ProcessManager
         */
        global $pm;
        $pm->wakeup();
    });
    $server->on('request', function (openswoole_http_request $request, openswoole_http_response $response) {
        $response->end("OK\n");
    });
    $server->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
OK
hello swooler
