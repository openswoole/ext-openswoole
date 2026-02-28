--TEST--
openswoole_http2_server: trailer
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$pm = new ProcessManager;
$pm->initRandomData(MAX_REQUESTS);
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($cli->connect());
        /** @var $channels OpenSwoole\Coroutine\Channel[] */
        $channels = [];
        for ($n = MAX_REQUESTS; $n--;) {
            $streamId = $cli->send(new OpenSwoole\Http2\Request);
            if (Assert::greaterThan($streamId, 0)) {
                $channels[$streamId] = $channel = new OpenSwoole\Coroutine\Channel;
                go(function () use ($streamId, $channel) {
                    /** @var $response OpenSwoole\Http2\Response */
                    $response = $channel->pop();
                    Assert::same($response->streamId, $streamId);
                    $headers = $response->headers;
                    unset($headers['date']);
                    Assert::same($headers, [
                        'content-type' => 'application/srpc',
                        'trailer' => 'srpc-status, srpc-message',
                        'server' => 'OpenSwoole '. OPENSWOOLE_VERSION,
                        'content-length' => '32',
                        'srpc-status' => '0',
                        'srpc-message' => '',
                    ]);
                });
            }
        }
        while (true) {
            /** @var $response OpenSwoole\Http2\Response */
            $response = $cli->recv();
            $channels[$response->streamId]->push($response);
            unset($channels[$response->streamId]);
            if (empty($channels)) {
                break;
            }
        }
    });
    OpenSwoole\Event::wait();
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'worker_num' => 1,
        'log_file' => '/dev/null',
        'open_http2_protocol' => true
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (OpenSwoole\Http\Request $request, OpenSwoole\Http\Response $response) use ($pm) {
        $response->header('content-type', 'application/srpc');
        $response->header('trailer', 'srpc-status, srpc-message');
        $trailer = [
            "srpc-status" => '0',
            "srpc-message" => ''
        ];
        foreach ($trailer as $trailer_name => $trailer_value) {
            $response->trailer($trailer_name, $trailer_value);
        }
        $response->end($pm->getRandomData());
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
