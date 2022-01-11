<?php

function fetchUrl($url) {
    $urlInfo = parse_url($url);
    $host = $urlInfo['host'];
    $path = $urlInfo['path'];
    $client = new Swoole\Coroutine\HTTP\Client($host, 443, true);
    $client->setHeaders([
        'Host' => $host,
        "User-Agent" => 'Chrome/49.0.2587.3',
        'Accept' => 'text/html,application/xhtml+xml,application/xml',
        'Accept-Encoding' => 'gzip',
    ]);
    $client->set(['timeout' => 1]);
    $client->get($path);
    $client->close();

    return strlen($client->body);
}

Co\run(function () {
    $chan1 = new chan(1);
    $chan2 = new chan(1);

    go(function() use ($chan1) {
        $content = fetchUrl('https://openswoole.com/');
        $chan1->push(['content' => $content, 'id' => 'chan1']);
    });

    go(function() use ($chan2) {
        $content = fetchUrl('https://openswoole.com/');
        $chan2->push(['content' => $content, 'id' => 'chan2']);
    });
    var_dump([$chan1, $chan2]);
    $i = 0;

    $start = time();
    $ret = co::select([$chan1, $chan2], [], 10);
    echo "select result:".var_export($ret, true)."\n";
    $cost = time() - $start;
    echo "SELECT DONE after $cost seconds\n";
    var_dump(array_values($ret['pull_chans'])[0]->pop());
    echo "waiting for shutdown..\n";
});