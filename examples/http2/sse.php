<?php
Swoole\Coroutine::set([
    'trace_flags' => SWOOLE_TRACE_HTTP2,
    'log_level' => 0,
]);
$http = new swoole_http_server("0.0.0.0", 9501, SWOOLE_BASE, SWOOLE_SOCK_TCP);
$http->set([
    'open_http2_protocol' => 1,
    'enable_static_handler' => TRUE,
    'document_root' => dirname(__DIR__),
    'enable_coroutine' => true,
]);

$http->on('request', function (swoole_http_request $request, swoole_http_response $response) {
	$response->header('Access-Control-Allow-Origin', '*');
    $response->header('Content-Type', 'text/event-stream');
    $response->header('Cache-Control', 'no-cache');
    $response->header('X-Accel-Buffering', 'no');

    while(1) {
        $response->write("event: ping\n");
        $r = $response->write("data: {\"time\": \"" . date(DATE_ISO8601) . "\"}\n\n");
        if(!$r) return;
        co::sleep(1);
    }
    
    $response->end("<h1>Hello Swoole</h1>");
});

$http->start();
