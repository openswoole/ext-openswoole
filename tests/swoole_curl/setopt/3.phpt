--TEST--
swoole_curl/setopt: curl_setopt() call with CURLOPT_HTTPHEADER
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);

require __DIR__ . '/../../include/bootstrap.php';

$cm = new \SwooleTest\CurlManager();
$cm->disableNativeCurl();
$cm->run(function ($host) {
    // start testing
    echo "*** curl_setopt() call with CURLOPT_HTTPHEADER\n";

    $url = "{$host}/";
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_HTTPHEADER, 1);

    $curl_content = curl_exec($ch);
    curl_close($ch);

    var_dump( $curl_content );

    $ch = curl_init();

    ob_start(); // start output buffering
    curl_setopt($ch, CURLOPT_HTTPHEADER, array());
    curl_setopt($ch, CURLOPT_URL, $host);

    $curl_content = curl_exec($ch);
    ob_end_clean();
    curl_close($ch);

    var_dump( $curl_content );

});


?>
--EXPECTF--
*** curl_setopt() call with CURLOPT_HTTPHEADER

Deprecated: Use SWOOLE_HOOK_NATIVE_CURL instead of SWOOLE_HOOK_CURL in @swoole-src/library/ext/curl.php on line %d

Warning: swoole_curl_setopt(): You must pass either an object or an array with the CURLOPT_HTTPHEADER argument in %s on line %d
bool(false)

Deprecated: Use SWOOLE_HOOK_NATIVE_CURL instead of SWOOLE_HOOK_CURL in @swoole-src/library/ext/curl.php on line %d
bool(true)
