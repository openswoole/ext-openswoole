--TEST--
openswoole_coroutine_system: waitEvent
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine;

$server = SwooleTest\LocalServer::start();

Coroutine::run(function () use ($server) {
    $client = stream_socket_client("tcp://{$server->host}:{$server->httpPort}", $errno, $errstr, 30);
    $events = Coroutine::waitEvent($client, SWOOLE_EVENT_READ | SWOOLE_EVENT_WRITE);
    Assert::eq($events, SWOOLE_EVENT_WRITE);
    fwrite($client, "GET / HTTP/1.1\r\nHost: {$server->host}\r\n\r\n");
    $events = Coroutine::waitEvent($client, SWOOLE_EVENT_READ);
    Assert::eq($events, SWOOLE_EVENT_READ);
    $response = fread($client, 8192);
    Assert::contains($response, 'HTTP/1.1 200 OK');
});

echo "DONE\n";

?>
--EXPECT--
DONE
