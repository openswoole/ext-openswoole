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

Co::set(['enable_coroutine' => false]);

$fp = new Client(SWOOLE_SOCK_TCP);

// sync connect
$fp->connect('www.qq.com', 80);

Event::add($fp, function($fp) {
    $resp = $fp->recv(8192);
    Assert::contains($resp, 'Location: https://www.qq.com/');

    Event::del($fp);
    $fp->close();

    echo "Done\n";
});

Event::write($fp, "GET / HTTP/1.1\r\nHost: www.qq.com\r\n\r\n");
Event::wait();
?>
--EXPECT--
Done
