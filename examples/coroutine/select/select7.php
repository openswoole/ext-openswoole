<?php declare(strict_types = 1);

// Example of pipeline

Co::set(['hook_flags' => SWOOLE_HOOK_ALL]);

function fetchUrl($url) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_HEADER, true);
    $response = curl_exec($ch);
    $header_size = curl_getinfo($ch, CURLINFO_HEADER_SIZE);
    $header = substr($response, 0, $header_size);
    curl_close($ch);
    return $header;
}

Co\run(function () {
    $chan1 = new chan(1);
    $chan2 = new chan(1);

    go(function() use ($chan1) {
        $header = fetchUrl('https://openswoole.com/');
        $chan1->push(['id' => 'chan1', 'header' => $header]);
    });

    go(function() use ($chan2) {
        $header = fetchUrl('https://www.google.com/');
        $chan2->push(['id' => 'chan2', 'header' => $header]);
    });

    go(function() use ($chan1, $chan2){
        while(1) {
            $ret = co::select([$chan1, $chan2], [], 10);
            $ret = array_values($ret['pull_chans'])[0]->pop();
            var_dump($ret);
        }
    });
});