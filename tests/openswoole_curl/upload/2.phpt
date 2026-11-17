--TEST--
openswoole_curl/upload: CURL file uploading
--INI--
--SKIPIF--
<?php require __DIR__ . '/../../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';
require_once TESTS_LIB_PATH . '/vendor/autoload.php';

use OpenSwoole\Runtime;



$server = SwooleTest\LocalServer::start();
Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () use ($server) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $server->url('/post', true));
    curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $file = new CurlFile(TEST_IMAGE);
    curl_setopt($ch, CURLOPT_POSTFIELDS, array("openswoole_file" => $file));
    $result = curl_exec($ch);
    Assert::notEmpty($result);
    $json = json_decode($result);
    Assert::notEmpty($json);
    Assert::notEmpty($json->files->openswoole_file);
    // binary files come back as a base64 data URI (httpbin format)
    $prefix = 'data:application/octet-stream;base64,';
    $fileContent = $json->files->openswoole_file;
    Assert::true(str_starts_with($fileContent, $prefix));
    $fileContent = substr($fileContent, strlen($prefix));
    Assert::eq(md5(base64_decode($fileContent)), md5_file(TEST_IMAGE));
});
?>
--EXPECTF--
