--TEST--
openswoole_client_coro: getsockname
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

Co::run(
    function () use ($server) {
        $conn = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($conn->connect($server->host, $server->httpPort));
        $info = $conn->getsockname();
        Assert::assert(filter_var($info['host'], FILTER_VALIDATE_IP));
        Assert::greaterThan($info['port'], 0);
    }
);
?>
--EXPECT--
