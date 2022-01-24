<?php
Swoole\Coroutine::set([
    'trace_flags' => SWOOLE_TRACE_HTTP2,
    'log_level' => 0,
]);
$key_dir = __DIR__ . '/../ssl/';
$http = new swoole_http_server("0.0.0.0", 9501, SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
$http->set([
    'open_http2_protocol' => 1,
    'enable_static_handler' => TRUE,
    'document_root' => __DIR__,
    'ssl_cert_file' => $key_dir . '/example.com+4.pem',
    'ssl_key_file' => $key_dir . '/example.com+4-key.pem',
]);

$http->on('request', function (swoole_http_request $request, swoole_http_response $response) {
	$response->header('Test-Value', [
        "a\r\n",
        'd5678',
        "e  \n ",
        null,
        5678,
        3.1415926,
    ]);
    $response->end("<h1>Hello Open Swoole</h1>");
});

$http->start();
