--TEST--
openswoole_server: bind
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
skip_if_in_valgrind();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;
use OpenSwoole\Timer;
use OpenSwoole\Event;

$simple_tcp_server = __DIR__ . "/../include/api/openswoole_server/opcode_server.php";
$port = get_one_free_port();

start_server($simple_tcp_server, TCP_SERVER_HOST, $port);
$timer = suicide(2000);
usleep(500 * 1000);

makeCoTcpClient(TCP_SERVER_HOST, $port, function (Client $cli) use ($port) {
    $r = $cli->send(opcode_encode("bind", [2, 42]));
    Assert::assert($r !== false);
}, function (Client $cli, $recv) use ($timer) {
    list($op, $binded) = opcode_decode($recv);
    Assert::assert($binded);
    $cli->close();
    Timer::clear($timer);
    echo "SUCCESS\n";
});

Event::wait();

?>
--EXPECT--
SUCCESS
