<?php
$key_dir = __DIR__ . '/../ssl/';
$http = new swoole_http_server("127.0.0.1", 9501, SWOOLE_BASE, SWOOLE_SOCK_TCP | SWOOLE_SSL);
$http->set([
    'open_http2_protocol' => 1,
    'enable_static_handler' => TRUE,
    'enable_coroutine' => TRUE,
    'document_root' => __DIR__,
    'ssl_cert_file' => $key_dir . '/example.com+4.pem',
    'ssl_key_file' => $key_dir . '/example.com+4-key.pem',
    // 'log_level' => SWOOLE_LOG_TRACE,
    // 'trace_flags' => SWOOLE_TRACE_ALL,
]);

$http->on('request', function (swoole_http_request $request, swoole_http_response $response) {
    $response->end("<h1>Hello Open Swoole</h1>");
});

$http->start();
