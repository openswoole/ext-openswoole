--TEST--
openswoole_client_coro: ssl client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

co::run(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
    if (!$cli->connect($server->host, $server->httpsPort)) {
        echo "ERROR\n";
    }

    $http = "GET / HTTP/1.0\r\nAccept: */*\r\nUser-Agent: Lowell-Agent\r\nHost: {$server->host}\r\nConnection: Close\r\n\r\n";
    if (!$cli->send($http)) {
        echo "ERROR\n";
    }

    $content = '';
    while (true) {
        $read = $cli->recv();
        if (empty($read)) {
            break;
        }
        $content .= $read;
    }
    $cli->close();
    Assert::assert(strpos($content, 'OpenSwoole test page') !== false);
});
?>
--EXPECT--
