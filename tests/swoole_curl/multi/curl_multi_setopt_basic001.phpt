--TEST--
swoole_curl/multi: curl_multi_setopt basic test
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID < 80000) die("Skipped: php version < 8."); ?>
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

Swoole\Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    $mh = curl_multi_init();
    var_dump(curl_multi_setopt($mh, CURLMOPT_PIPELINING, 0));

    try {
        curl_multi_setopt($mh, -1, 0);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
    curl_multi_close($mh);
});
?>
--EXPECTF--
bool(true)
curl_multi_setopt(): Argument #2 ($option) is not a valid cURL multi option
