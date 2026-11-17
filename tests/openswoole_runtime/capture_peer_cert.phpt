--TEST--
openswoole_runtime: ssl capture_peer_cert
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

function capture_peer_cert($host, $port)
{
    $g = stream_context_create([
        "ssl" => [
            "capture_peer_cert" => true,
            'capture_peer_cert_chain' => true,
            'verify_peer' => false,
            'verify_peer_name' => false,
        ]
    ]);
    $r = stream_socket_client("ssl://{$host}:{$port}", $errno, $errstr, 5, STREAM_CLIENT_CONNECT, $g);
    if (!$r) {
        return false;
    }
    $cont = stream_context_get_params($r);
    if (!$cont) {
        return false;
    }
    return $cont;
}

$server = SwooleTest\LocalServer::start();

OpenSwoole\Runtime::setHookFlags(SWOOLE_HOOK_ALL);

co::run(function () use ($server) {
    $result = capture_peer_cert($server->host, $server->httpsPort);
    Assert::isArray($result);
    $info1 = openssl_x509_parse($result["options"]["ssl"]["peer_certificate"]);
    Assert::isArray($info1);
    Assert::contains($info1['name'], 'CN=127.0.0.1');
    Assert::same($info1['subject']['CN'], '127.0.0.1');
    $chain = $result['options']['ssl']['peer_certificate_chain'];
    Assert::isArray($chain);
    Assert::greaterThanEq(count($chain), 1);
    Assert::same(openssl_x509_parse($chain[0])['subject']['CN'], '127.0.0.1');
});
?>
--EXPECT--
