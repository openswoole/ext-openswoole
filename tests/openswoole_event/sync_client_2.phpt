--TEST--
openswoole_event: sync client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Client;
use OpenSwoole\Event;

$fp = new Client(SWOOLE_SOCK_TCP);
$result = $fp->connect('openswoole.com', 80, 10);

Assert::true($result);
Assert::eq($fp->errCode, 0);

Event::add($fp, null, function (Client $fp) {
    $fp->send("GET / HTTP/1.1\r\nHost: openswoole.com\r\n\r\n");
    Event::set($fp, function ($fp) {
        $resp = $fp->recv(8192);
        Assert::contains($resp, 'Location: https://openswoole.com/');

        Event::del($fp);
        $fp->close();

        echo "Done\n";
    }, null, SWOOLE_EVENT_READ);
}, SWOOLE_EVENT_WRITE);

Event::wait();
?>
--EXPECT--
Done
