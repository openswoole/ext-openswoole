--TEST--
openswoole_http_server: header values with CR or LF
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Http\Server;

$pm = new SwooleTest\ProcessManager;
$pm->parentFunc = function () use ($pm) {
    co::run(function () use ($pm) {
        $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP);
        Assert::true($cli->connect('127.0.0.1', $pm->getFreePort()));
        $cli->send("GET /response HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n");
        $data = '';
        while (($chunk = $cli->recv()) !== '' && $chunk !== false) {
            $data .= $chunk;
        }
        $head = explode("\r\n\r\n", $data, 2)[0];
        echo 'status line: ', strtok($head, "\r\n"), "\n";
        echo 'extra header in response: ', stripos($data, 'X-Extra') !== false ? 'yes' : 'no', "\n";

        foreach ([
            'path' => function ($c) { return $c->get("/x HTTP/1.1\r\nX-Extra: 1\r\n\r\nGET /y"); },
            'header value' => function ($c) { $c->setHeaders(['X-A' => "a\r\nX-Extra: 1"]); return $c->get('/'); },
            'header key' => function ($c) { $c->setHeaders(["X-A\r\nX-Extra" => 'a']); return $c->get('/'); },
            'method' => function ($c) { $c->setMethod("GET /x HTTP/1.1\r\nX-Extra: 1\r\n\r\nGET"); return $c->execute('/'); },
        ] as $name => $request) {
            $c = new OpenSwoole\Coroutine\Http\Client('127.0.0.1', $pm->getFreePort());
            $ok = @$request($c);
            echo "client {$name}: ", $ok ? 'sent ' . $c->getBody() : 'rejected', "\n";
        }
    });
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new Server('127.0.0.1', $pm->getFreePort(), SWOOLE_BASE);
    $http->set(['log_file' => '/dev/null']);
    $http->on('workerStart', function () use ($pm) {
        $pm->wakeup();
    });
    $http->on('request', function ($request, $response) {
        if ($request->server['request_uri'] !== '/response') {
            // the client side: echo what arrived
            $response->end(isset($request->header['x-extra']) ? 'extra' : 'clean');
            return;
        }
        Assert::false(@$response->status(200, "OK\r\nX-Extra: 1"));
        Assert::false(@$response->cookie('a', 'b', 0, "/\r\nX-Extra: 1"));
        Assert::false(@$response->trailer("X-T\r\nX-Extra", 'v'));
        Assert::false(@$response->trailer('X-T', "v\r\nX-Extra: 1"));
        $response->header = ["X-H\r\nX-Extra" => 'v', 'X-Ok' => 'v'];
        @$response->end('body');
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
status line: HTTP/1.1 200 OK
extra header in response: no
client path: rejected
client header value: rejected
client header key: rejected
client method: rejected
