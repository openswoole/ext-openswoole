--TEST--
openswoole_curl/multi: Casting CurlHandle to int returns object ID
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
use OpenSwoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    $handle1 = curl_init();
    var_dump((int)$handle1);
    $handle2 = curl_init();
    var_dump((int)$handle2);

// NB: Unlike resource IDs, object IDs are reused.
    unset($handle2);
    $handle3 = curl_init();
    var_dump((int)$handle3);

// Also works for CurlMultiHandle.
    $handle4 = curl_multi_init();
    var_dump((int)$handle4);
});

?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
int(%d)
