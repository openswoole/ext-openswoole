--TEST--
openswoole_curl/multi: basic tests;
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php
if (!extension_loaded('curl')) { print("skip"); }
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

OpenSwoole\Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function() {
  $multiCurl = array();
  $result = array();
  $mh = curl_multi_init();
  $urls = ['https://openswoole.com','https://www.php.net/'];
  foreach ($urls as $i => $url) {
    $multiCurl[$i] = curl_init();
    curl_setopt($multiCurl[$i], CURLOPT_URL,$url);
    curl_setopt($multiCurl[$i], CURLOPT_HEADER,0);
    curl_setopt($multiCurl[$i], CURLOPT_RETURNTRANSFER,1);
    curl_multi_add_handle($mh, $multiCurl[$i]);
  }
  $index=0;
  do {
    curl_multi_exec($mh,$index);
  } while($index > 0);
  foreach($multiCurl as $k => $ch) {
    $result[$k] = strlen(curl_multi_getcontent($ch));
    curl_multi_remove_handle($mh, $ch);
  }
  // close
  curl_multi_close($mh);
  var_dump($result);
});
--EXPECTF--
array(2) {
  [0]=>
  int(%d)
  [1]=>
  int(%d)
}
