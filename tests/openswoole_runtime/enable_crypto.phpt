--TEST--
openswoole_runtime: stream_socket_enable_crypto
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

swoole\runtime::enableCoroutine();

go(function () use ($server) {
    $fp = stream_socket_client("tcp://{$server->host}:{$server->httpsPort}", $errno, $errstr, 30,
        STREAM_CLIENT_CONNECT, stream_context_create(['ssl' => ['verify_peer' => false, 'verify_peer_name' => false]]));
    if (!$fp) {
        echo "$errstr ($errno)<br />\n";
    } else {
        $http = "GET / HTTP/1.0\r\nAccept: */*User-Agent: Lowell-Agent\r\nHost: {$server->host}\r\nConnection: Close\r\n\r\n";
        stream_socket_enable_crypto($fp,true, STREAM_CRYPTO_METHOD_SSLv23_CLIENT);
        fwrite($fp, $http);
        $content = '';
        while (!feof($fp)) {
            $content .= fread($fp, 1024);
        }
        fclose($fp);
        Assert::assert(strpos($content,'OpenSwoole test page') !== false);
    }
});
openswoole_event_wait();
?>
--EXPECT--
