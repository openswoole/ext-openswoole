--TEST--
openswoole_client_sync: sendto
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Client;
use OpenSwoole\Constant;

$port = get_one_free_port();
$socket = stream_socket_server("udp://127.0.0.1:{$port}", $errno, $errstr, STREAM_SERVER_BIND);
Assert::notEmpty($socket);

$cli = new Client(Constant::SOCK_UDP);
Assert::true($cli->sendto('127.0.0.1', $port, 'packet-1'));
Assert::true($cli->sendto('localhost', $port, 'packet-2'));
Assert::false($cli->sendto('error_domain', $port, 'hello'));
Assert::same($cli->errCode, Constant::ERROR_DNSLOOKUP_RESOLVE_FAILED);
Assert::true($cli->sendto('localhost', $port, 'packet-3'));

foreach (['packet-1', 'packet-2', 'packet-3'] as $expected) {
    $read = [$socket];
    $write = $except = null;
    Assert::same(stream_select($read, $write, $except, 5), 1);
    Assert::same(stream_socket_recvfrom($socket, 1024, 0, $peer), $expected);
    Assert::startsWith($peer, '127.0.0.1:');
}
echo "DONE\n";
?>
--EXPECTF--
Warning: OpenSwoole\Client::sendto(): sendto to server[error_domain:%d] failed. Error: DNS Lookup resolve failed[%d] in %ssendto.php on line %d
DONE
