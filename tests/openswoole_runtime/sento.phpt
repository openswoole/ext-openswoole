--TEST--
openswoole_runtime: stream_socket_sendto
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

swoole\runtime::enableCoroutine();

go(function () use ($server) {
    $fp = stream_socket_client("tcp://{$server->host}:{$server->httpPort}", $errno, $errstr, 30);
    if (!$fp) {
        echo "$errstr ($errno)<br />\n";
    } else {
        $http = "GET / HTTP/1.0\r\nAccept: */*User-Agent: Lowell-Agent\r\nHost: {$server->host}\r\nConnection: Close\r\n\r\n";
        stream_socket_sendto($fp, $http);
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
