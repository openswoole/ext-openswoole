--TEST--
openswoole_websocket_server: on('handshake') on the server and a port
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

error_reporting(E_ALL);

$server = new OpenSwoole\WebSocket\Server('127.0.0.1', 0);
$server->on('handshake', function () {});
$port = $server->listen('127.0.0.1', 0, SWOOLE_SOCK_TCP);
$port->on('handshake', function () {});
echo "done\n";
?>
--EXPECT--
done
