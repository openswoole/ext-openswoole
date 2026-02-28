--TEST--
openswoole_server: send message
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
use OpenSwoole\Server;

$simple_tcp_server = __DIR__ . "/../include/api/openswoole_server/opcode_server.php";
$port = get_one_free_port();

start_server($simple_tcp_server, TCP_SERVER_HOST, $port);

$timer = suicide(2000);
usleep(500 * 1000);

makeCoTcpClient(TCP_SERVER_HOST, $port, function(Client $cli) use($port) {
    $r = $cli->send(opcode_encode("sendMessage", ["SUCCESS", 1]));
    Assert::assert($r !== false);
}, function(Client $cli, $recv) {
    list($op, $msg) = opcode_decode($recv);
    echo $msg;
    global $timer;
    $cli->close();
    Timer::clear($timer);
});
OpenSwoole\Event::wait();

?>
--EXPECT--
SUCCESS
