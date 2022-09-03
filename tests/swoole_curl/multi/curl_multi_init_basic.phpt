--TEST--
swoole_curl/multi: Test curl_multi_init()
--CREDITS--
Mark van der Velden
#testfest Utrecht 2009
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
use Swoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    // start testing
    echo "*** Testing curl_multi_init(void); ***\n";

    //create the multiple cURL handle
    $mh = curl_multi_init();

    curl_type_assert($mh, 'Swoole-Coroutine-cURL-Multi-Handle', CurlMultiHandle::class);

    curl_multi_close($mh);
    curl_type_assert($mh, 'Swoole-Coroutine-cURL-Multi-Handle', CurlMultiHandle::class);
});
?>
--EXPECTF--
*** Testing curl_multi_init(void); ***
