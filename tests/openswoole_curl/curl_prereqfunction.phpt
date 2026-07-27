--TEST--
openswoole_curl: CURLOPT_PREREQFUNCTION support (required for Guzzle 7.12+)
--SKIPIF--
<?php
require __DIR__ . '/../include/skipif.inc';
if (!defined('CURLOPT_PREREQFUNCTION')) {
    skip('CURLOPT_PREREQFUNCTION not available (requires libcurl >= 7.80.0)');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Runtime;
use OpenSwoole\Coroutine\Scheduler;

Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

$scheduler = new Scheduler();
$scheduler->add(function () {
    $ch = curl_init();

    // Test 1: Setting CURLOPT_PREREQFUNCTION to null (Guzzle handle release pattern)
    echo "Test 1: Set CURLOPT_PREREQFUNCTION to null\n";
    $result = curl_setopt($ch, CURLOPT_PREREQFUNCTION, null);
    var_dump($result);

    // Test 2: Setting CURLOPT_PREREQFUNCTION to a callable
    echo "Test 2: Set CURLOPT_PREREQFUNCTION to callable\n";
    $result = curl_setopt($ch, CURLOPT_PREREQFUNCTION, function ($ch, $primaryIp, $localIp, $primaryPort, $localPort) {
        return CURL_PREREQFUNC_OK;
    });
    var_dump($result);

    // Test 3: Setting CURLOPT_PREREQFUNCTION back to null (reset)
    echo "Test 3: Reset CURLOPT_PREREQFUNCTION to null\n";
    $result = curl_setopt($ch, CURLOPT_PREREQFUNCTION, null);
    var_dump($result);

    curl_close($ch);
    echo "Done\n";
});
$scheduler->start();
?>
--EXPECT--
Test 1: Set CURLOPT_PREREQFUNCTION to null
bool(true)
Test 2: Set CURLOPT_PREREQFUNCTION to callable
bool(true)
Test 3: Reset CURLOPT_PREREQFUNCTION to null
bool(true)
Done
