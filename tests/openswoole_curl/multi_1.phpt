--TEST--
openswoole_curl: multi 1
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
require_once TESTS_API_PATH.'/curl_multi.php';

use OpenSwoole\Runtime;



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);
co::run(function () {
    openswoole_test_curl_multi();
    echo "Done\n";
});
?>
--EXPECT--
Done
