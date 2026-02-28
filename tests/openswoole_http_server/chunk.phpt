--TEST--
openswoole_http_server: http chunk
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;

$pm->parentFunc = function () use ($pm) {
    go(function () use ($pm) {
        $data = httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        Assert::assert(!empty($data));
        Assert::assert(md5($data) === md5_file(TEST_IMAGE));
        $pm->kill();
    });
    OpenSwoole\Event::wait();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm) {
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);

    $http->set([
        //'log_file' => '/dev/null',
    ]);

    $http->on("WorkerStart", function ($serv, $wid) {
        global $pm;
        $pm->wakeup();
    });

    $http->on("request", function (openswoole_http_request $request,  openswoole_http_response $response) {
        $data = str_split(file_get_contents(TEST_IMAGE), 8192);
        foreach ($data as $chunk)
        {
            $response->write($chunk);
        }
        $response->end();
    });

    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
