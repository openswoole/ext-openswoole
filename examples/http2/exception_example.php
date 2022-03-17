<?php
Swoole\Coroutine::set([
    'trace_flags' => SWOOLE_TRACE_HTTP2,
    'log_level' => 0,
]);
$key_dir = __DIR__ . '/../ssl/';
$http = new swoole_http_server("0.0.0.0", 9501, SWOOLE_PROCESS, SWOOLE_SOCK_TCP | SWOOLE_SSL);
$http->set([
    'open_http2_protocol' => 1,
    'enable_static_handler' => TRUE,
    'document_root' => __DIR__,
    'enable_coroutine' => true,
    'ssl_cert_file' => $key_dir . '/example.com+4.pem',
    'ssl_key_file' => $key_dir . '/example.com+4-key.pem',
    'log_level' => SWOOLE_LOG_TRACE,
    'trace_flags' => SWOOLE_TRACE_ALL,
]);


$http->on('request', function (swoole_http_request $request, swoole_http_response $response) {

    $response->header('Access-Control-Allow-Origin', '*');
    $response->header('Content-Type', 'text/event-stream');
    $response->header('Cache-Control', 'no-cache');
    $response->header('X-Accel-Buffering', 'no');

    while(1) {
        $response->write("event: ping\n\n");
        $r = $response->write("data: {\"time\": \"" . date(DATE_ISO8601) . "\"}\n\n");
        if(!$r) return;
        co::sleep(1);
        throw new \Exception();
    }

    $response->end(__DIR__);
});

$http->start();
