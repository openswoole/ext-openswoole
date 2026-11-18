--TEST--
openswoole_http_server: cookies with an expiry time also get Max-Age
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($cli->get('/'));
        foreach ($cli->set_cookie_headers as $header) {
            echo preg_replace('/expires=[^;]+/', 'expires=DATE', $header), "\n";
        }
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null']);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        $response->cookie('a', '1', time() + 3600, '/', '', true, true, 'Lax');
        $response->cookie('b', '2', time() - 10);
        $response->cookie('c', '3');
        $response->cookie('d', '');
        $response->end();
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECTREGEX--
a=1; expires=DATE; Max-Age=(3599|3600); path=\/; secure; httponly; samesite=Lax
b=2; expires=DATE; Max-Age=0
c=3
d=deleted; expires=DATE; Max-Age=0
