--TEST--
openswoole_http_server: http_autoindex lists large directories and escapes file names
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$dir = sys_get_temp_dir() . '/openswoole_autoindex_large';
@mkdir("$dir/www/list", 0777, true);
// about 200KB of names
for ($i = 0; $i < 2000; $i++) {
    touch(sprintf('%s/www/list/%s-%04d.txt', $dir, str_repeat('n', 80), $i));
}
touch("$dir/www/list/<b>name<\\b>.txt");

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $res = httpRequest("http://127.0.0.1:{$pm->getFreePort()}/list/");
        echo $res['statusCode'], "\n";
        echo substr_count($res['body'], '<li>'), "\n";
        echo strpos($res['body'], '<b>') === false ? 'escaped' : 'raw', "\n";
        echo strpos($res['body'], '&lt;b&gt;name&lt;\\b&gt;.txt') !== false ? 'listed' : 'missing', "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm, $dir) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set([
        'log_file' => '/dev/null',
        'enable_static_handler' => true,
        'document_root' => "$dir/www",
        'http_autoindex' => true,
    ]);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->end('dynamic');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
exec('rm -rf ' . escapeshellarg($dir));
?>
--EXPECT--
200
2002
escaped
listed
