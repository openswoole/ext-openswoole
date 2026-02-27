--TEST--
openswoole_curl: https
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Runtime;

const N = 8;

Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
$s = microtime(true);
co::run(function () {
    $n = N;
    while($n--) {
        go(function() {
            $ch = curl_init();
            $code = uniqid('openswoole_');
                $domain = 'www.google.com';
            $url = "https://{$domain}/?code=".urlencode($code);

            curl_setopt($ch, CURLOPT_URL, $url);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
            curl_setopt($ch, CURLOPT_HEADER, 0);
            curl_setopt($ch, CURLOPT_CONNECTTIMEOUT , 2);
            curl_setopt($ch, CURLOPT_TIMEOUT, 5);
            curl_setopt($ch, CURLOPT_HEADERFUNCTION, function ($ch, $strHeader) {
                return strlen($strHeader);
            });

            $output = curl_exec($ch);
            if ($output === false) {
                echo "CURL Error:" . curl_error($ch);
            }
            Assert::notEmpty($output);
            Assert::greaterThan(strlen($output), 9000);
            curl_close($ch);
        });
    }
});
echo "Done\n";
?>
--EXPECT--
Done
