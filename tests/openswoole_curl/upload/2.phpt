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



Runtime::enableCoroutine(SWOOLE_HOOK_NATIVE_CURL);

co::run(function () {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://httpbun.com/anything");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    $file = new CurlFile(TEST_IMAGE);
    curl_setopt($ch, CURLOPT_POSTFIELDS, array("openswoole_file" => $file));
    $result = curl_exec($ch);
    Assert::notEmpty($result);
    $json = json_decode($result);
    Assert::notEmpty($json);
    Assert::notEmpty($json->files->openswoole_file);
    // httpbun.com returns files as object {content, filename, headers, size}
    // content is base64-encoded (without data: URI prefix)
    $fileContent = is_object($json->files->openswoole_file)
        ? $json->files->openswoole_file->content
        : $json->files->openswoole_file;
    // Strip data URI prefix if present (httpbin format)
    $prefix = 'data:application/octet-stream;base64,';
    if (is_string($fileContent) && str_starts_with($fileContent, $prefix)) {
        $fileContent = substr($fileContent, strlen($prefix));
    }
    Assert::eq(md5(base64_decode($fileContent)), md5_file(TEST_IMAGE));
});
?>
--EXPECTF--
