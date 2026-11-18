--TEST--
openswoole_http_client_coro: http client set basic auth
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$server = SwooleTest\LocalServer::start();
go(function () use ($server) {
    $cli = new OpenSwoole\Coroutine\Http\Client($server->host, $server->httpPort);
    $cli->set(['timeout' => 10]);
    $cli->setHeaders([
        'host' => $server->host,
        'User-Agent' => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip',
    ]);
    $username = get_safe_random();
    $password = get_safe_random();
    $cli->setBasicAuth($username, $password);
    $ret = $cli->get("/basic-auth/{$username}/{$password}");
    if ($ret && !empty($cli->statusCode === 200)) {
        echo "OK\n";
    } else {
        echo "ERROR\n";
    }
});
OpenSwoole\Event::wait();
?>
--EXPECT--
OK
