--TEST--
swoole_curl/basic: Test curl_error() & curl_errno() function with problematic proxy
--CREDITS--
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

$cm = new \SwooleTest\CurlManager();
$cm->run(function ($host) {
    $url = "http://www.example.org";
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_PROXY, uniqid() . ":1234");
    curl_setopt($ch, CURLOPT_URL, $url);

    curl_exec($ch);
    var_dump(curl_error($ch));
    var_dump(curl_errno($ch));
    curl_close($ch);

}, false);

?>
--EXPECTF--
string(%d) "%r(Couldn't resolve proxy|Could not resolve proxy:|Could not resolve host:|Could not resolve:)%r %s"
int(5)
