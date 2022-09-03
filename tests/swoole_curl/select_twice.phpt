--TEST--
swoole_curl: select twice
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
require_once TESTS_API_PATH.'/curl_multi.php';

use Swoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    $n = 4;
    while ($n--) {
        go(function () {
            swoole_test_curl_multi(['select_twice' => true]);
            echo "Done\n";
        });
    }
});
?>
--EXPECTF--
Fatal error: Uncaught OpenSwoole\Error: cURL is executing, cannot be operated in %s:%d
Stack trace:
#0 %s(%d): curl_multi_select(%s)
%A
  thrown in %s on line %d
