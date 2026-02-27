--TEST--
openswoole_http_server: http_compression
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm)
{
    go(function () use ($pm) {
        try {
            $data =  httpGetBody("http://127.0.0.1:{$pm->getFreePort()}/");
        } catch(Exception $e) {
            Assert::contains($e->getMessage(), 'Connection reset by peer');
        }
        $pm->kill();
    });
    OpenSwoole\Event::wait();
    echo "DONE\n";
};

$pm->childFunc = function () use ($pm)
{
    $http = new openswoole_http_server('127.0.0.1', $pm->getFreePort());

    $http->set([
        'http_compression' => false,
        'log_file' => '/dev/null',
        'buffer_output_size' => 128 * 1024,
    ]);

    $http->on("WorkerStart", function ($serv, $wid) use ($pm) {
        $pm->wakeup();
    });

    $http->on("request", function ($request, openswoole_http_response $response) {
        Assert::eq($response->end(str_repeat('A', 256 * 1024)), false);
        Assert::eq(OpenSwoole\Util::getLastErrorCode(), SWOOLE_ERROR_DATA_LENGTH_TOO_LARGE);
    });

    $http->start();
};

$pm->childFirst();
$pm->run();
?>
--EXPECT--
DONE
