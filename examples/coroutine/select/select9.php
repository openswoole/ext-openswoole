<?php declare(strict_types = 1);
Co::set(['hook_flags' => SWOOLE_HOOK_ALL]);

function fetchUrl($url) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $output = curl_exec($ch);
    curl_close($ch);
    return $output;
}

Co\run(function () {
    $chan1 = new chan(1);
    $chan2 = new chan(1);

    go(function() use ($chan1) {
        $content = fetchUrl('https://openswoole.com/');
        $chan1->push(['content' => $content, 'chan' => 'chan1']);
    });

    go(function() use ($chan2) {
        $content = fetchUrl('https://www.google.com/');
        $chan2->push(['content' => $content, 'chan' => 'chan2']);
    });

    $ret = co::select([$chan1, $chan2], [], 10);
    $content = array_values($ret['pull_chans'])[0]->pop();
    var_dump($content);
});