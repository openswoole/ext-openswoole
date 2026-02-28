<?php

Co::set(['use_fiber_context' => true]);
$server = new OpenSwoole\HTTP\Server("127.0.0.1", 9501);

$server->on("start", function (OpenSwoole\Http\Server $server) {
    echo "OpenSwoole http server is started at http://127.0.0.1:9501\n";
});

$server->on("request", function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) {
    $response->header("Content-Type", "text/plain");
    exit();
    return $response->end("Hello World\n");
});

$server->start();