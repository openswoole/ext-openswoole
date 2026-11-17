--TEST--
openswoole_client_coro: connect with dns
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// a local listener; "localhost" is resolved through /etc/hosts, not outside DNS
$server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
$port = (int) explode(':', stream_socket_get_name($server, false))[1];

Co::run(function () use ($port) {
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
    Assert::true($cli->connect('localhost', $port));
});

?>
--EXPECT--
