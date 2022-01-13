<?php

Swoole\Runtime::enableCoroutine();

Co\run(function () {

    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://openswoole.com");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    $output = curl_exec($ch);
    if ($output === FALSE) {
        echo "CURL Error:" . curl_error($ch);
    }
    curl_close($ch);
    echo strlen($output) . "bytes\n";
});

swoole_event_wait();