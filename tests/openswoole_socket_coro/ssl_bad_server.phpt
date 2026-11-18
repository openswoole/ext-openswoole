--TEST--
openswoole_socket_coro: ssl connect with bad server
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// the HTTP port of the local server does not speak TLS
$server = SwooleTest\LocalServer::start();

Co::set(['log_level' => SWOOLE_LOG_WARNING]);

co::run(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Socket(AF_INET, SOCK_STREAM, 0);
    $cli->setProtocol(['open_ssl' => true,]);

    Assert::false ($cli->connect($server->host, $server->httpPort));
    Assert::eq($cli->errCode, SWOOLE_ERROR_SSL_HANDSHAKE_FAILED);
});
?>
--EXPECT--
