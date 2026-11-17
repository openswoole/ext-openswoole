--TEST--
openswoole_client_coro: ssl verify
--SKIPIF--
<?php 
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
skip_if_extension_not_exist('openssl');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Coroutine;

$dir = sys_get_temp_dir() . '/openswoole_client_coro_ssl_verify_' . getmypid();
@mkdir($dir);
$key = openssl_pkey_new(['private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
$csr = openssl_csr_new(['commonName' => '127.0.0.1'], $key, ['digest_alg' => 'sha256']);
$cert = openssl_csr_sign($csr, null, $key, 1, ['digest_alg' => 'sha256']);
openssl_x509_export_to_file($cert, "$dir/cert.pem");
openssl_pkey_export_to_file($key, "$dir/key.pem");

Co::set(['log_file' => TEST_LOG_FILE]);
OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);

Coroutine::run(function () use ($dir) {
    $server = stream_socket_server('ssl://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
        stream_context_create(['ssl' => ['local_cert' => "$dir/cert.pem", 'local_pk' => "$dir/key.pem"]]));
    Assert::assert($server);
    $port = (int) explode(':', stream_socket_get_name($server, false))[1];
    go(function () use ($server) {
        for ($i = 0; $i < 2; $i++) {
            $conn = @stream_socket_accept($server, 5);
            if ($conn) {
                @fread($conn, 1);
                @fclose($conn);
            }
        }
    });

    // the server certificate is trusted
    $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $client->set([
        'ssl_cafile' => "$dir/cert.pem",
        'ssl_verify_peer' => true,
    ]);
    $ret = $client->connect('127.0.0.1', $port);
    Assert::true($ret);
    $client->close();

    // the server certificate is not signed by the given CA
    $client2 = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $client2->set([
        'ssl_cafile' => SSL_FILE_DIR . '/mosquitto.org.crt',
        'ssl_verify_peer' => true,
    ]);
    $ret = $client2->connect('127.0.0.1', $port);
    Assert::false($ret);
    Assert::eq($client2->errCode, SWOOLE_ERROR_SSL_VERIFY_FAILED);
});

@unlink("$dir/cert.pem");
@unlink("$dir/key.pem");
@rmdir($dir);
?>
--EXPECT--
