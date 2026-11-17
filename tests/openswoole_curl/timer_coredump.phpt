--TEST--
openswoole_curl: timer coredump
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Runtime;



$server = SwooleTest\LocalServer::start();
Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

function test()
{
    global $server;
    $curl = curl_init();
    curl_setopt($curl, CURLOPT_URL, $server->url('/', true));
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_exec($curl);
    curl_close($curl);
}

co::run('test');
co::run('test');

echo "Done\n";
?>
--EXPECT--
Done
