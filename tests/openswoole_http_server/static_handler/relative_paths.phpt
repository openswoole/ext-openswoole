--TEST--
openswoole_http_server/static_handler: relative request paths
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Http\Server;

$base = sys_get_temp_dir() . '/openswoole_static_paths';
@mkdir("$base/www/sub", 0777, true);
@mkdir("$base/other", 0777, true);
@mkdir("$base/abc", 0777, true);
file_put_contents("$base/www/index.txt", 'PUBLIC');
file_put_contents("$base/other/file.txt", 'OTHER');
file_put_contents("$base/abc/f.txt", 'SIBLING');

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        foreach (['/index.txt', '/sub/../index.txt', '/sub/%2e%2e/index.txt', '/%2e%2e/other/file.txt',
                     '/%2E%2E/other/file.txt', '/../other/file.txt', '/../abc/f.txt', '/index.txt%00.png'] as $path) {
            $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
            Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
            $cli->send("GET {$path} HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
            $data = '';
            while (($chunk = $cli->recv()) !== '' && $chunk !== false) {
                $data .= $chunk;
            }
            echo $path, ' ', explode("\r\n\r\n", $data, 2)[1], "\n";
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
?>
--EXPECT--
/index.txt PUBLIC
/sub/../index.txt PUBLIC
/sub/%2e%2e/index.txt PUBLIC
/%2e%2e/other/file.txt DYNAMIC
/%2E%2E/other/file.txt DYNAMIC
/../other/file.txt DYNAMIC
/../abc/f.txt DYNAMIC
/index.txt%00.png DYNAMIC
