<?php

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
    go(function() {
        $content = fetchUrl('https://openswoole.com');
        var_dump(strlen($content));
    });

    go(function() {
        $content = fetchUrl('https://openswoole.com');
        var_dump(strlen($content));
    });
});