--TEST--
openswoole_http_client_coro: get twice and keepalive
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use OpenSwoole\Coroutine\Http\Client;
const N = 2;

$server = SwooleTest\LocalServer::start();
co::run(function () use ($server) {
    $client = new Client($server->host, $server->httpsPort, true);
    $client->set(['timeout' => 5,]);
    for ($i = 0; $i < N; $i++) {
        $rand = mt_rand(100000, 999999999);
        $path = "/get?email=" . $rand . "@" . substr(md5((string)microtime(true)), 0, 8) . ".com";
        Assert::assert($client->get($path));
        Assert::assert($client->getStatusCode() == 200);
    }
});

?>
--EXPECT--
