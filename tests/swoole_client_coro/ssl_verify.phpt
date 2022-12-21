--TEST--
swoole_client_coro: ssl verify
--SKIPIF--
<?php 
require __DIR__ . '/../include/skipif.inc';
skip_if_no_ssl();
skip_if_offline();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
use Swoole\Coroutine;

Co::set(['log_file' => TEST_LOG_FILE]);

Coroutine::run(function () {
    $client = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $client->set([
        //'ssl_cafile' => SSL_FILE_DIR . '/mosquitto.org.crt',
        'ssl_verify_peer' => true,
    ]);
    $ret = $client->connect('php.net', 443);
    Assert::true($ret);

    $client2 = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_TCP | SWOOLE_SSL);
    $client2->set([
        'ssl_cafile' => SSL_FILE_DIR . '/mosquitto.org.crt',
        'ssl_verify_peer' => true,
    ]);
    $ret = $client2->connect('openswoole.com', 443);
    Assert::false($ret);
    Assert::eq($client2->errCode, SWOOLE_ERROR_SSL_VERIFY_FAILED);
});

?>
--EXPECT--
