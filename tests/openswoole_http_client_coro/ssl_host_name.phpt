--TEST--
openswoole_http_client_coro: https client with ssl_host_name
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_extension_not_exist('openssl');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
OpenSwoole\Coroutine::set(['log_level' => SWOOLE_LOG_ERROR]);

$dir = sys_get_temp_dir() . '/openswoole_http_client_ssl_host_name';
@mkdir($dir);
$key = openssl_pkey_new(['private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
$csr = openssl_csr_new(['commonName' => 'localhost'], $key, ['digest_alg' => 'sha256']);
$cert = openssl_csr_sign($csr, null, $key, 1, ['digest_alg' => 'sha256']);
openssl_x509_export_to_file($cert, "$dir/cert.pem");
openssl_pkey_export_to_file($key, "$dir/key.pem");

co::run(function () use ($dir) {
    $server = stream_socket_server('ssl://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
        stream_context_create(['ssl' => ['local_cert' => "$dir/cert.pem", 'local_pk' => "$dir/key.pem"]]));
    Assert::assert($server);
    $port = (int) explode(':', stream_socket_get_name($server, false))[1];
    $running = true;
    go(function () use ($server, &$running) {
        while ($running) {
            // a client that rejects the certificate fails the handshake here
            $conn = @stream_socket_accept($server, 1);
            if ($conn) {
                @fread($conn, 8192);
                $body = 'OpenSwoole verify peer';
                @fwrite($conn, "HTTP/1.1 200 OK\r\nContent-Length: " . strlen($body) . "\r\nConnection: close\r\n\r\n{$body}");
                @fclose($conn);
            }
        }
    });

    // the certificate is issued to localhost, so that name verifies
    $c = new OpenSwoole\Coroutine\http\Client('127.0.0.1', $port, true);
    $c->set([
        'timeout' => 5,
        'ssl_verify_peer' => true,
        'ssl_allow_self_signed' => true,
        'ssl_host_name' => 'localhost'
    ]);
    $c->get('/');
    Assert::assert(strlen($c->body) > 0);
    Assert::same($c->statusCode, 200);

    // and another name does not
    $c = new OpenSwoole\Coroutine\http\Client('127.0.0.1', $port, true);
    $c->set([
        'timeout' => 5,
        'ssl_verify_peer' => true,
        'ssl_allow_self_signed' => true,
        'ssl_host_name' => 'example.com'
    ]);
    Assert::false(@$c->get('/'));
    Assert::same($c->statusCode, SWOOLE_HTTP_CLIENT_ESTATUS_CONNECT_FAILED);

    $running = false;
});
@unlink("$dir/cert.pem");
@unlink("$dir/key.pem");
@rmdir($dir);
?>
--EXPECT--
