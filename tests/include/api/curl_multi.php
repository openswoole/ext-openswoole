<?php
// started before co::run(), which the tests call this from
$GLOBALS['curl_multi_server'] = $GLOBALS['curl_multi_server'] ?? SwooleTest\LocalServer::start();

function openswoole_test_curl_multi($options = []) {
    $server = $GLOBALS['curl_multi_server'];
    $ch1 = curl_init();
    $ch2 = curl_init();

    // a slower request, so the transfers are still running while the test selects
    curl_setopt($ch1, CURLOPT_URL, $server->url('/delay/0.1', true));
    curl_setopt($ch1, CURLOPT_HEADER, 0);
    curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch1, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch1, CURLOPT_SSL_VERIFYHOST, 0);

    curl_setopt($ch2, CURLOPT_URL, $server->url('/', true));
    curl_setopt($ch2, CURLOPT_HEADER, 0);
    curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch2, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch2, CURLOPT_SSL_VERIFYHOST, 0);

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

    Assert::contains(curl_multi_getcontent($ch1), '/delay/0.1');
    Assert::contains(curl_multi_getcontent($ch2), 'OpenSwoole test page');

    curl_multi_remove_handle($mh, $ch1);
    curl_multi_remove_handle($mh, $ch2);

    curl_multi_close($mh);
}
