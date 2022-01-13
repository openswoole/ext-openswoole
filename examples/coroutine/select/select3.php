<?php declare(strict_types = 1);

// Example of select and curl, sleep

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
        $start = time();
        co:sleep(3);
        $content = fetchUrl('https://openswoole.com/');
        var_dump(strlen($content));
        $cost = time() - $start;
        echo "chan1 DONE after $cost seconds\n";
        $chan1->push(['content' => $content, 'id' => 'chan1']);
    });

    go(function() use ($chan2) {
        $start = time();
        co:sleep(2);
        $content = fetchUrl('https://www.google.com/');
        var_dump(strlen($content));
        $cost = time() - $start;
        echo "chan2 DONE after $cost seconds\n";
        $chan2->push(['content' => $content, 'id' => 'chan2']);
    });

    $start = time();
    echo "start select\n";
    $ret = co::select([$chan1, $chan2], [], 10);
    echo "select result:".var_export($ret, true)."\n";
    $cost = time() - $start;
    echo "SELECT DONE after $cost seconds\n";

    echo "waiting for shutdown..\n";
});