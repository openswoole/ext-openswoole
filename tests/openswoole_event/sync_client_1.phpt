--TEST--
openswoole_event: sync client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Client;
use OpenSwoole\Event;

Co::set(['enable_coroutine' => false]);

$server = SwooleTest\LocalServer::start();

$fp = new Client(SWOOLE_SOCK_TCP);

// sync connect
$fp->connect($server->host, $server->httpPort);

Event::add($fp, function($fp) {
    $resp = $fp->recv(8192);
    Assert::contains($resp, 'HTTP/1.1 200 OK');

    Event::del($fp);
    $fp->close();

    echo "Done\n";
});

Event::write($fp, "GET / HTTP/1.1\r\nHost: {$server->host}\r\n\r\n");
Event::wait();
?>
--EXPECT--
Done
