--TEST--
openswoole_http_server: cookie values are encoded and decoded like PHP does
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        // a cookie set by another application: "+" is kept, "%2B" and "%20" are decoded
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $cli->send("GET /read HTTP/1.1\r\nHost: localhost\r\nCookie: csrf=qDGK+Vj/R3S=; b=x+y%2Bz; c=a%20b\r\nConnection: close\r\n\r\n");
        $data = '';
        while (($chunk = $cli->recv(5)) !== '' && $chunk !== false) {
            $data .= $chunk;
        }
        echo explode("\r\n\r\n", $data, 2)[1], "\n";

        // Response::cookie() and the client keep the value as it is
        $http = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
        Assert::true($http->get('/set'));
        echo $http->set_cookie_headers[0], "\n";
        echo json_encode($http->cookies), "\n";
        Assert::true($http->get('/read'));
        echo $http->body, "\n";
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $server = new OpenSwoole\Http\Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $server->set(['log_file' => '/dev/null']);
    $server->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $server->on('request', function ($request, $response) {
        if ($request->server['request_uri'] === '/set') {
            $response->cookie('v', 'a b+c/=');
            $response->end();
            return;
        }
        $response->end(json_encode($request->cookie));
    });
    $server->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
{"csrf":"qDGK+Vj\/R3S=","b":"x+y+z","c":"a b"}
v=a%20b%2Bc%2F%3D
{"v":"a b+c\/="}
{"v":"a b+c\/="}
