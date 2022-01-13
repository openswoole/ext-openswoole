<?php declare(strict_types = 1);

// Example of go concurrent without channel

Co::set(['hook_flags' => SWOOLE_HOOK_ALL]);

function fetchUrl($url) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $output = curl_exec($ch);
    var_dump(strlen($output));
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