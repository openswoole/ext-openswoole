--TEST--
swoole_socket_coro/setopt: setOption IPV6_PKTINFO
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$socket = new OpenSwoole\Coroutine\Socket(AF_INET6, SOCK_DGRAM, SOL_UDP);

var_dump(@$socket->setOption(IPPROTO_IPV6, IPV6_PKTINFO, []));
var_dump($socket->setOption(IPPROTO_IPV6, IPV6_PKTINFO, [
    "addr" => '::1',
    "ifindex" => 0
]));

?>
--EXPECTF--
bool(false)
bool(true)
