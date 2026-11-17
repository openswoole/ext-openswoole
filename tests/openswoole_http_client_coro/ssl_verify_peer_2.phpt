--TEST--
openswoole_http_client_coro: ssl_verify_peer [2]
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_extension_not_exist('openssl');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Http\Client;

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
OpenSwoole\Coroutine::set(['log_level' => SWOOLE_LOG_ERROR]);

$dir = sys_get_temp_dir() . '/openswoole_http_client_verify_peer_2';
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

    // a self-signed certificate is accepted only with ssl_allow_self_signed
    $client = new Client('127.0.0.1', $port, true);
    $client->set([
        'timeout' => 5,
        'ssl_verify_peer' => true,
        'ssl_allow_self_signed' => true,
    ]);
    $result = $client->get("/");
    Assert::eq($result, true);
    Assert::eq($client->getStatusCode(), 200);
    $info = openssl_x509_parse($client->getPeerCert());
    Assert::contains($info['name'], 'localhost');
    Assert::contains($client->getBody(), 'OpenSwoole');

    $client = new Client('127.0.0.1', $port, true);
    $client->set([
        'timeout' => 5,
        'ssl_verify_peer' => true,
    ]);
    Assert::false(@$client->get("/"));
    Assert::same($client->getStatusCode(), SWOOLE_HTTP_CLIENT_ESTATUS_CONNECT_FAILED);

    $running = false;
});
@unlink("$dir/cert.pem");
@unlink("$dir/key.pem");
@rmdir($dir);
?>
--EXPECT--
