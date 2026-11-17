--TEST--
openswoole_socket_coro/setopt: setOption SO_RCVTIMEO
--DESCRIPTION--
-wrong params
-set/get params comparison
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

// fixed ports collide when tests run in parallel
$port = get_one_free_port();

$socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, SOL_TCP);
$socket->bind("127.0.0.1", $port);

Assert::assert($socket->setOption(SOL_SOCKET, SO_REUSEADDR, true));
Assert::assert($socket->setOption(SOL_SOCKET, SO_REUSEPORT, true));

?>
--EXPECTF--
