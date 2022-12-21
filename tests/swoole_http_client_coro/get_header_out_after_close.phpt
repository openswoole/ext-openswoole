--TEST--
swoole_http_client_coro: getHeaderOut after close
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use Swoole\Coroutine as co;

co::create(function () {
    $http = new OpenSwoole\Coroutine\Http\Client('httpbin.org', 80, false);
    $http->set([
        'timeout' => -1,
        'keep_alive' => false,
    ]);
    $http->execute('/get');
    echo strpos($http->getHeaderOut(), 'httpbin.org') > -1;
});
swoole_event::wait();

?>
--EXPECT--
1
