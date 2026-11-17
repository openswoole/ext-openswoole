--TEST--
openswoole_runtime: stream context pass null
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$server = SwooleTest\LocalServer::start();

swoole\runtime::enableCoroutine();
go(function() use ($server) {
   //This function internal send null stream context parameter to `php_stream_open_wrapper_ex`
   $md5 = md5_file($server->url('/'));
   var_dump(!empty($md5));
   Assert::eq($md5, md5('<html><head><title>OpenSwoole test page</title></head><body>' .
       str_repeat('<p>OpenSwoole test page</p>', 150) . '</body></html>'));
});
openswoole_event_wait();

?>
--EXPECT--
bool(true)
