--TEST--
openswoole_client_coro: enableSSL
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Client;

$server = SwooleTest\LocalServer::start();

co::run(function () use ($server) {
    $client = new Client(SWOOLE_SOCK_TCP);
    Assert::true($client->connect($server->host, $server->httpsPort));
    Assert::true($client->enableSSL());

    $http = "GET / HTTP/1.0\r\nAccept: */*\r\nUser-Agent: Lowell-Agent\r\nHost: {$server->host}\r\nConnection: Close\r\n\r\n";
    if (!$client->send($http)) {
        echo "ERROR\n";
    }

    $content = '';
    while (true) {
        $read = $client->recv();
        if (empty($read)) {
            break;
        }
        $content .= $read;
    }
    $client->close();
    Assert::assert(strpos($content, 'OpenSwoole test page') !== false);
});
?>
--EXPECT--
