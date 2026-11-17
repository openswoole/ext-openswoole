--TEST--
openswoole_runtime/ssl: ssl:// clients verify the peer certificate and name by default
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
skip_if_extension_not_exist('openssl');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$dir = sys_get_temp_dir() . '/openswoole_verify_peer_default';
@mkdir($dir);
$key = openssl_pkey_new(['private_key_bits' => 2048, 'private_key_type' => OPENSSL_KEYTYPE_RSA]);
$csr = openssl_csr_new(['commonName' => 'localhost'], $key, ['digest_alg' => 'sha256']);
$cert = openssl_csr_sign($csr, null, $key, 1, ['digest_alg' => 'sha256']);
openssl_x509_export_to_file($cert, "$dir/cert.pem");
openssl_pkey_export_to_file($key, "$dir/key.pem");

OpenSwoole\Runtime::enableCoroutine(OpenSwoole\Runtime::HOOK_ALL);
OpenSwoole\Coroutine::set(['log_level' => SWOOLE_LOG_ERROR]);
co::run(function () use ($dir) {
    $server = stream_socket_server('ssl://127.0.0.1:0', $errno, $errstr, STREAM_SERVER_BIND | STREAM_SERVER_LISTEN,
        stream_context_create(['ssl' => ['local_cert' => "$dir/cert.pem", 'local_pk' => "$dir/key.pem"]]));
    Assert::assert($server);
    $port = (int) explode(':', stream_socket_get_name($server, false))[1];
    $cases = [
        'default' => [],
        'self signed, matching name' => ['allow_self_signed' => true, 'peer_name' => 'localhost'],
        'self signed, wrong name' => ['allow_self_signed' => true, 'peer_name' => 'example.com'],
        'self signed, wrong name, verify_peer_name off' => ['allow_self_signed' => true, 'peer_name' => 'example.com',
            'verify_peer_name' => false],
        'verify_peer off' => ['verify_peer' => false],
    ];
    go(function () use ($server, $cases) {
        foreach ($cases as $case) {
            $conn = @stream_socket_accept($server, 5);
            if ($conn) {
                @fwrite($conn, 'hello');
                @fclose($conn);
            }
        }
    });
    foreach ($cases as $name => $options) {
        $client = @stream_socket_client("ssl://127.0.0.1:{$port}", $errno, $errstr, 5, STREAM_CLIENT_CONNECT,
            stream_context_create(['ssl' => $options]));
        echo $name, ': ', $client && fread($client, 5) === 'hello' ? 'connected' : 'rejected', "\n";
    }
});
@unlink("$dir/cert.pem");
@unlink("$dir/key.pem");
@rmdir($dir);
?>
--EXPECT--
default: rejected
self signed, matching name: connected
self signed, wrong name: rejected
self signed, wrong name, verify_peer_name off: connected
verify_peer off: connected
