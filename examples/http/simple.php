<?php

use Swoole\Http\Server;
use Swoole\Http\Request;
use Swoole\Http\Response;

$server = new Swoole\HTTP\Server("127.0.0.1", 9501);

$server->on("Start", function(Server $server)
{
    echo "Swoole http server is started at http://127.0.0.1:9501\n";
});

class A {
    public $a = 'abcd';
};

$server->on("Request", function(Request $request, Response $response) use ($server)
{
    // memory leak example
    global $c;
    $c[] = new A();
    $response->header("Content-Type", "text/plain");
    $response->end(json_encode($server->stats()));
});

$server->start();