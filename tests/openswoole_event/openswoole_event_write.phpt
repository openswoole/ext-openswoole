--TEST--
openswoole_event: openswoole_event_write
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

$fp = stream_socket_client("tcp://{$server->host}:{$server->httpPort}", $errno, $errstr, 30);

openswoole_event_add($fp, function($fp) {
    $resp = fread($fp, 8192);

    //socket处理完成后，从epoll事件中移除socket
    openswoole_event_del($fp);
    fclose($fp);

    echo "SUCCESS\n";
});

openswoole_event_write($fp, "GET / HTTP/1.1\r\nHost: {$server->host}\r\n\r\n");

echo "Finish\n";
\OpenSwoole\Event::wait();
?>
--EXPECT--
Finish
SUCCESS
