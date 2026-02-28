--TEST--
openswoole_curl/multi: curl_multi_errno and curl_multi_strerror basic test
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (PHP_VERSION_ID >= 80000) die("Skipped: php version >= 8."); ?>
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
use OpenSwoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    $mh = curl_multi_init();
    $errno = curl_multi_errno($mh);
    echo $errno . PHP_EOL;
    echo curl_multi_strerror($errno) . PHP_EOL;

    try {
        curl_multi_setopt($mh, -1, -1);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }

    $errno = curl_multi_errno($mh);
    echo $errno . PHP_EOL;
    echo curl_multi_strerror($errno) . PHP_EOL;
});
?>
--EXPECTF--
0
No error

Warning: curl_multi_setopt(): Invalid curl multi configuration option in %s on line %d
6
Unknown option
