<?php
$serv = new \swoole_http_server("127.0.0.1", 9503, SWOOLE_BASE);

$serv->on('request', function ($req, $resp) {
    $chan = new chan(2);
    go(function () use ($chan) {
        $cli = new Swoole\Coroutine\Http\Client('www.google.com', 443, true);
            $cli->set(['timeout' => 10]);
            $cli->setHeaders([
            'Host' => "www.google.com",
            "User-Agent" => 'Chrome/49.0.2587.3',
            'Accept' => 'text/html,application/xhtml+xml,application/xml',
            'Accept-Encoding' => 'gzip',
        ]);
        $ret = $cli->get('/');
        $chan->push(['www.google.com' => substr(trim(strip_tags($cli->body)), 0, 100)]);
    });

    go(function () use ($chan) {
        $cli = new Swoole\Coroutine\Http\Client('www.php.net', 443, true);
        $cli->set(['timeout' => 10]);
        $cli->setHeaders([
            'Host' => "www.php.net",
            "User-Agent" => 'Chrome/49.0.2587.3',
            'Accept' => 'text/html,application/xhtml+xml,application/xml',
            'Accept-Encoding' => 'gzip',
        ]);
        $ret = $cli->get('/');
        $chan->push(['www.php.net' => substr(trim(strip_tags($cli->body)), 0, 100)]);
    });

    $result = [];
    for ($i = 0; $i < 2; $i++)
    {
        $result += $chan->pop();
    }
    $resp->header('Content-Type', 'text/html;charset=utf-8');
    $resp->end(var_export($result, true));
});
$serv->start();
