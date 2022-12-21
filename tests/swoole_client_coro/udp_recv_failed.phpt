--TEST--
swoole_client_coro: udp recv failed
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

co::run(function () {
    $cli = new OpenSwoole\Coroutine\Client(SWOOLE_SOCK_UDP);
    $ret = $cli->connect('127.0.0.1', get_one_free_port(), 3);
    echo "connect ret:".var_export($ret,true)."\n";

    $ret = $cli->send("hello");
    echo "send ret:".var_export($ret,true)."\n";

    $ret = @$cli->recv();
    echo "recv ret:".var_export($ret,true)."\n";
    Assert::same($cli->errCode, SOCKET_ECONNREFUSED);
    $cli->close();
});
?>
--EXPECT--
connect ret:true
send ret:5
recv ret:false
