--TEST--
openswoole_http_client_coro: https client
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpsPort, true);
    $cli->set(['timeout' => 10]);
    $cli->setHeaders([
        'Host' => $server->host,
        'User-Agent' => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip',
    ]);
    $ret = ($cli->get('/'));
    if (!$ret) {
        echo("ERROR\n");
        var_dump($cli->errCode);
        return;
    } else {
        echo("OK\n");
        $cli->close();
    }
});
openswoole_event::wait();
?>
--EXPECT--
OK
