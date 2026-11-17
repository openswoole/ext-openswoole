--TEST--
openswoole_http_server/static_handler: HEAD requests
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Http\Server;

$base = sys_get_temp_dir() . '/openswoole_static_head';
@mkdir("$base/www/list", 0777, true);
file_put_contents("$base/www/index.txt", 'PUBLIC');
touch("$base/www/list/a.txt");

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        foreach (['GET /index.txt', 'HEAD /index.txt', 'HEAD /list/', 'HEAD /missing.txt'] as $line) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $cli->send("{$line} HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
            $data = '';
            while (($chunk = $cli->recv(5)) !== '' && $chunk !== false) {
                $data .= $chunk;
            }
            [$head, $body] = explode("\r\n\r\n", $data, 2);
            preg_match('/^Content-Length: (\d+)/mi', $head, $m);
            echo $line, ': ', strtok($head, "\r\n"), ', length ', $m[1] > 0 ? 'set' : 'zero', ', body ', strlen($body), "\n";
        }
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm, $base) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'enable_static_handler' => true,
        'document_root' => "$base/www",
        'http_autoindex' => true,
        'static_handler_locations' => ['/'],
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end('DYNAMIC');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
exec('rm -rf ' . escapeshellarg($base));
?>
--EXPECT--
GET /index.txt: HTTP/1.1 200 OK, length set, body 6
HEAD /index.txt: HTTP/1.1 200 OK, length set, body 0
HEAD /list/: HTTP/1.1 200 OK, length set, body 0
HEAD /missing.txt: HTTP/1.1 404 Not Found, length set, body 0
