<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
use Swoole\Runtime;



$fn = function() {
    $transfers = 1;
    $callback = function($parent, $passed) use (&$transfers) {
        curl_setopt($passed, CURLOPT_WRITEFUNCTION, function ($ch, $data) {
            echo "Received ".strlen($data);
            return strlen($data);
        });
        $transfers++;
        return CURL_PUSH_OK;
    };
    $mh = curl_multi_init();
    curl_multi_setopt($mh, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);
    curl_multi_setopt($mh, CURLMOPT_PUSHFUNCTION, $callback);
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, 'https://h2demo:4430/serverpush');
    curl_setopt($ch, CURLOPT_HTTP_VERSION, 3);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_multi_add_handle($mh, $ch);
    $active = null;
    do {
        $status = curl_multi_exec($mh, $active);
        // echo "active=$active, status=$status\n";
        do {
            $info = curl_multi_info_read($mh);
            if (false !== $info && $info['msg'] == CURLMSG_DONE) {
                $handle = $info['handle'];
                if ($handle !== null) {
                    $transfers--;
                    curl_multi_remove_handle($mh, $handle);
                    curl_close($handle);
                }
            }
        } while ($info);
    } while ($transfers);
    curl_multi_close($mh);
};

if (swoole_array_default_value($argv, 1) == 'ori') {
    $fn();
} else {
    Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
    co::run($fn);
}
?>
