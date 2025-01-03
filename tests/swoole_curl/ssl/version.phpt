--TEST--
swoole_curl/ssl: Test SSL_VERSION option
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php declare(strict_types = 1);

require __DIR__ . '/../../include/bootstrap.php';

$cm = new \SwooleTest\CurlManager();
$cm->run(function ($host) {
    $ch = curl_init();

    curl_setopt($ch, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_URL, "https://openswoole.com/");
    $result = curl_exec($ch);
    Assert::assert($result);
    Assert::contains($result, 'openswoole');
    curl_close($ch);

}, false);

?>
--EXPECTF--
