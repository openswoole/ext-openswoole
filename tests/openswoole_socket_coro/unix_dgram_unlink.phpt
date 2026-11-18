--TEST--
openswoole_socket_coro: closing a unix dgram client keeps the server's socket file
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Coroutine\Socket;

co::run(function () {
    $path = sys_get_temp_dir() . '/openswoole_unix_dgram_unlink.sock';
    @unlink($path);

    $server = new Socket(AF_UNIX, SOCK_DGRAM, 0);
    Assert::true($server->bind($path));

    $client = new Socket(AF_UNIX, SOCK_DGRAM, 0);
    Assert::true($client->connect($path));
    Assert::same($client->send('hello'), 5);
    Assert::same($server->recv(1024, 1), 'hello');
    $client->close();
    unset($client);
    var_dump(file_exists($path));

    $server->close();
    unset($server);
    var_dump(file_exists($path));
});
?>
--EXPECT--
bool(true)
bool(false)
