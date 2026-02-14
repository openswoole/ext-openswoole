--TEST--
swoole_global: socket construct check
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
co::run(function () {
    $socket = new class (1, 2, 3) extends OpenSwoole\Coroutine\Socket
    {
        public function __construct($domain, $type, $protocol)
        {
            // parent::__construct($domain, $type, $protocol); // without parent call
        }
    };
    $socket->connect('127.0.0.1', 12345);
});
?>
--EXPECTF--
Fatal error: OpenSwoole\Coroutine\Socket::connect(): you must call Socket constructor first in %s on line %d
%A
