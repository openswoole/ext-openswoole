<?php
function swoole_test_curl_multi($options = []) {
    $ch1 = curl_init();
    $ch2 = curl_init();

    curl_setopt($ch1, CURLOPT_URL, "https://www.google.com/");
    curl_setopt($ch1, CURLOPT_HEADER, 0);
    curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);

    curl_setopt($ch2, CURLOPT_URL, "https://openswoole.com/");
    curl_setopt($ch2, CURLOPT_HEADER, 0);
    curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);

    $mh = curl_multi_init();

    curl_multi_add_handle($mh, $ch1);
    curl_multi_add_handle($mh, $ch2);

    $active = null;
    do {
        $mrc = curl_multi_exec($mh, $active);
    } while ($mrc == CURLM_CALL_MULTI_PERFORM);

    if (isset($options['select_twice'])) {
        if (isset($options['sleep'])) {
            unset($options['sleep']);
        }
        go(function() use($mh) {
            Co::usleep(5000);
            curl_multi_select($mh);
        });
    }

    if (isset($options['usleep'])) {
        Co::usleep($options['usleep']);
    }

    while ($active && $mrc == CURLM_OK) {
        $n = curl_multi_select($mh);
        if ($n != -1) {
            do {
                $mrc = curl_multi_exec($mh, $active);
            } while ($mrc == CURLM_CALL_MULTI_PERFORM);
        }
    }

    $info1 = curl_multi_info_read($mh);
    $info2 = curl_multi_info_read($mh);
    $info3 = curl_multi_info_read($mh);

    Assert::eq($info1['msg'], CURLMSG_DONE);
    Assert::eq($info2['msg'], CURLMSG_DONE);
    Assert::eq($info3, false);

    Assert::contains(curl_multi_getcontent($ch1), 'google');
    Assert::contains(curl_multi_getcontent($ch2), 'OpenSwoole');

    curl_multi_remove_handle($mh, $ch1);
    curl_multi_remove_handle($mh, $ch2);

    curl_multi_close($mh);
}
