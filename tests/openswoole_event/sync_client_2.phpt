--TEST--
openswoole_event: sync client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Client;
use OpenSwoole\Event;

$server = SwooleTest\LocalServer::start();

$fp = new Client(SWOOLE_SOCK_TCP);
$result = $fp->connect($server->host, $server->httpPort, 10);

Assert::true($result);
Assert::eq($fp->errCode, 0);

Event::add($fp, null, function (Client $fp) use ($server) {
    $fp->send("GET /get HTTP/1.1\r\nHost: {$server->host}\r\n\r\n");
    Event::set($fp, function ($fp) {
        $resp = $fp->recv(8192);
        Assert::contains($resp, 'HTTP/1.1 200 OK');

        Event::del($fp);
        $fp->close();

        echo "Done\n";
    }, null, SWOOLE_EVENT_READ);
}, SWOOLE_EVENT_WRITE);

Event::wait();
?>
--EXPECT--
Done
