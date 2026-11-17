--TEST--
openswoole_runtime: stream context
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
    $bindPort = get_one_free_port();
    $opts = array(
        'socket' => array(
            'bindto' => "0:{$bindPort}",
        ),
    );
    $ctx = stream_context_create($opts);
    $fp = stream_socket_client("tcp://{$server->host}:{$server->httpPort}", $errno, $errstr, 30, STREAM_CLIENT_CONNECT, $ctx);
    if (!$fp) {
        echo "$errstr ($errno)<br />\n";
    } else {
        Assert::eq((int) explode(':', stream_socket_get_name($fp, false))[1], $bindPort);
        fwrite($fp, "GET / HTTP/1.0\r\nHost: {$server->host}\r\nAccept: */*\r\n\r\n");
        $content = '';
        stream_set_timeout($fp, 5, 30000);
        while (!feof($fp)) {
            $content .= fread($fp, 8192);
        }
        fclose($fp);
        Assert::assert(strpos($content,'OpenSwoole test page') !== false);
    }
});
openswoole_event_wait();
?>
--EXPECT--
