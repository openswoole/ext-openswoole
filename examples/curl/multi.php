<?php

function test()
{
    $ch1 = curl_init();
    $ch2 = curl_init();

    curl_setopt($ch1, CURLOPT_URL, "https://openswoole.com/");
    curl_setopt($ch1, CURLOPT_HEADER, 0);
    curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);

    curl_setopt($ch2, CURLOPT_URL, "https://www.php.net/");
    curl_setopt($ch2, CURLOPT_HEADER, 0);
    curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);

    $mh = curl_multi_init();

    curl_multi_add_handle($mh, $ch1);
    curl_multi_add_handle($mh, $ch2);

    echo "add \n";

    $active = null;

    do {
        $mrc = curl_multi_exec($mh, $active);
        echo "exec[1], retval=$mrc\n";
    } while ($mrc == CURLM_CALL_MULTI_PERFORM);

    while ($active && $mrc == CURLM_OK) {
        $n = curl_multi_select($mh);
        echo "select, retval=$n\n";
        if ($n != -1) {
            do {
                $mrc = curl_multi_exec($mh, $active);
                echo "exec[2], retval=$mrc, active=$active\n";
            } while ($mrc == CURLM_CALL_MULTI_PERFORM);
        }
    }


    var_dump(strlen(curl_multi_getcontent($ch1)));
    var_dump(strlen(curl_multi_getcontent($ch2)));

    curl_multi_remove_handle($mh, $ch1);
    curl_multi_remove_handle($mh, $ch2);
    curl_multi_close($mh);
}

if (empty($argv[1])) {
    Co\run(function () {
        test();
    });
} else {
    test();
}

