--TEST--
openswoole_curl/setopt: CURLOPT_PROTOCOLS_STR and CURLOPT_REDIR_PROTOCOLS_STR
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
skip_if_constant_not_defined('CURLOPT_PROTOCOLS_STR');
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Runtime;

Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () {
    $ch = curl_init();
    var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, 'http,https'));
    var_dump(curl_setopt($ch, CURLOPT_REDIR_PROTOCOLS_STR, 'http,https'));

    // The option must reach libcurl, not only be accepted: with https alone, an http URL fails
    // before any connection is attempted.
    $ch = curl_init('http://127.0.0.1:1/');
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_PROTOCOLS_STR, 'https');
    var_dump(curl_exec($ch));
    var_dump(curl_errno($ch) === CURLE_UNSUPPORTED_PROTOCOL);
});
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
Done
