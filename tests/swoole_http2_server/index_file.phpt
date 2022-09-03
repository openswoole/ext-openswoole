--TEST--
swoole_http2_server: index file handler
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Http\Server;
use Swoole\Http\Response;
use Swoole\Http\Request;

define('DOC_ROOT', realpath(__DIR__ . '/../..'));

$pm = new ProcessManager;
$pm->parentFunc = function ($pid) use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http2\Client('127.0.0.1', $pm->getFreePort(), false);
        $cli->set([
            'timeout' => -1,
        ]);
        $cli->connect();

        $req = new OpenSwoole\Http2\Request;
        $req->path = '/';
        //request
        for ($n = MAX_REQUESTS; $n--;) {
            Assert::assert($cli->send($req));
        }
        //response
        for ($n = MAX_REQUESTS; $n--;) {
            $response = $cli->recv();
            Assert::same($response->statusCode, 200);
            Assert::same(md5_file(DOC_ROOT . '/README.md'), md5($response->data));
        }
    });
    $pm->kill();
    echo "DONE\n";
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE, SWOOLE_SOCK_TCP);
    $http->set([
        'log_file' => '/dev/null',
        'open_http2_protocol' => true,
        'enable_static_handler' => true,
        'http_index_files' => ['README.md'],
        'document_root' => DOC_ROOT,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function (Request $request, Response $response) use ($http) {
        $response->end('hello world');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();

?>
--EXPECT--
DONE
