--TEST--
openswoole_http_client_coro: getHeaderOut after close
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine as co;

$server = SwooleTest\LocalServer::start();
co::create(function () use ($server) {
    $http = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpPort, false);
    $http->set([
        'timeout' => -1,
        'keep_alive' => false,
    ]);
    $http->execute('/get');
    echo strpos($http->getHeaderOut(), $server->host) > -1;
});
openswoole_event::wait();

?>
--EXPECT--
1
