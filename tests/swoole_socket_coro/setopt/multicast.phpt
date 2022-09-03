--TEST--
swoole_socket_coro/setopt: multicast
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$socket = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_DGRAM, SOL_UDP);
$socket->bind('0.0.0.0', 9905);

$ret = $socket->setOption(IPPROTO_IP, MCAST_JOIN_GROUP, array(
    'group' => '224.10.20.30',
    'interface' => 0
));

if ($ret === false)
{
    throw new RuntimeException('Unable to join multicast group');
}

go(function () use ($socket) {
    $n = 10;
    while($n--) {
        $addr = [];
        $data = $socket->recvfrom($addr);
        Assert::assert(strlen($data) > 10);
        Assert::assert(!empty($addr['port']));
        Assert::assert(!empty($addr['address']));
    }
});

go(function () use ($socket) {
    $client = new Co\Client(SWOOLE_SOCK_UDP);
    $client->connect('224.10.20.30', 9905);
    $n = 10;
    while($n--) {
        $client->send("hello world [$n]\n");
        co::usleep(30000);
    }
});

swoole_event_wait();

?>
--EXPECTF--
