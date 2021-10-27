--TEST--
swoole_curl/basic: Test curl_init() function with basic functionality
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (!extension_loaded("curl")) exit("skip curl extension not loaded"); ?>
--FILE--
<?php

require __DIR__ . '/../../include/bootstrap.php';

$cm = new \SwooleTest\CurlManager();
$cm->disableNativeCurl();
$cm->run(function ($host) {
    $ch = curl_init();
    Assert::isInstanceOf($ch, Swoole\Curl\Handler::class);

}, false);

?>
===DONE===
--EXPECT--
Deprecated: Use SWOOLE_HOOK_NATIVE_CURL instead of SWOOLE_HOOK_CURL in @swoole-src/library/ext/curl.php on line 16
===DONE===
