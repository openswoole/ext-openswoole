--TEST--
swoole_channel_coro: no ctor
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

class MyChan extends Swoole\Coroutine\Channel {
    function __construct($size = null) {

    }
}

co::run(function () {
   $chan = new MyChan(100);
    $chan->pop();
});

?>
--EXPECTF--
Fatal error: OpenSwoole\Coroutine\Channel::pop(): you must call Channel constructor first in %s on line %d%A
