--TEST--
swoole_curl: error
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
$s = microtime(true);
co::run(function () {
    $ch = curl_init();
    $code = uniqid('swoole_');
    $url = "http://127.0.0.1:49494/?code=".urlencode($code);

    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_HEADER, 0);
    curl_setopt($ch, CURLOPT_HEADERFUNCTION, function ($ch, $strHeader) {
        return strlen($strHeader);
    });

    $output = curl_exec($ch);
    Assert::isEmpty($output);
    Assert::eq(curl_errno($ch), CURLE_COULDNT_CONNECT);
    Assert::contains(curl_error($ch), 'Failed to connect');
    $info = curl_getinfo($ch);
    Assert::isArray($info);
    Assert::eq($info['http_code'], 0);
    curl_close($ch);
});
echo "Done\n";
?>
--EXPECT--
Done
