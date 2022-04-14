--TEST--
swoole_http_server: bug cookie parse
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
define('COOKIE', 'Cookie: PHPSESSID=5359a08f4ddbf825f0e99a3393e5dc9e; HttpOnly; q=URVVma5UgEDm9RmQvBfXs7rCEG9hs9td9CXXmBRQ');
$pm = new ProcessManager;
$pm->parentFunc = function () use ($pm) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "http://127.0.0.1:" . $pm->getFreePort() . '/');
    $headers = [COOKIE];
    curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
    $result = curl_exec($ch);
    curl_close($ch);
    $pm->kill();
};
$pm->childFunc = function () use ($pm) {
    $http = new swoole_http_server('0.0.0.0', $pm->getFreePort());
    $http->set(array(
        'log_file' => '/dev/null',
    ));
    $http->on('request', function (swoole_http_request $request, swoole_http_response $response) {
        var_dump($request->cookie);
        $response->end();
    });
    $http->start();
};
$pm->childFirst();
$pm->run();
?>
--EXPECT--
array(3) {
  ["PHPSESSID"]=>
  string(32) "5359a08f4ddbf825f0e99a3393e5dc9e"
  ["HttpOnly"]=>
  string(0) ""
  ["q"]=>
  string(40) "URVVma5UgEDm9RmQvBfXs7rCEG9hs9td9CXXmBRQ"
}
