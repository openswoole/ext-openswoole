--TEST--
swoole_http_server: content length
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    go(function () use ($pm) {
        $headers = httpGetHeaders(
            "http://127.0.0.1:{$pm->getFreePort()}",
            [
                'headers' => ['Accept-Encoding' => 'gzip, br'],
                'data' => 'ABCD'
            ]
        );
        var_dump($headers);
        $pm->kill();
    });
    Swoole\Event::wait();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $http = new swoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);

    $http->set([
        //'log_file' => '/dev/null',
    ]);

    $http->on("WorkerStart", function ($serv, $wid) {
        global $pm;
        $pm->wakeup();
    });

    $http->on("request", function (swoole_http_request $request,  swoole_http_response $response) {
        $response->header('Content-Length', 10);
        $response->write("HELLO");
        $response->end();
    });

    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECTF--
array(4) {
  ["connection"]=>
  string(10) "keep-alive"
  ["content-type"]=>
  string(9) "text/html"
  ["date"]=>
  string(29) "%s"
  ["transfer-encoding"]=>
  string(7) "chunked"
}
DONE
