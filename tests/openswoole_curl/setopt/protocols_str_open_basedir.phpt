--TEST--
openswoole_curl/setopt: CURLOPT_PROTOCOLS_STR cannot enable FILE when an open_basedir is set
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
ini_set('open_basedir', __DIR__);

co::run(function () {
    $ch = curl_init();
    var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, 'FILE,http'));
    var_dump(curl_setopt($ch, CURLOPT_REDIR_PROTOCOLS_STR, 'all'));
    var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, 'http,https'));
    var_dump(curl_setopt_array($ch, [CURLOPT_PROTOCOLS_STR => 'file']));
    var_dump(curl_setopt_array($ch, [CURLOPT_PROTOCOLS => CURLPROTO_FILE]));
});
echo "Done\n";
?>
--EXPECTF--
Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)

Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)
bool(true)

Warning: curl_setopt_array(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)

Warning: curl_setopt_array(): CURLPROTO_FILE cannot be activated when an open_basedir is set in %s on line %d
bool(false)
Done
