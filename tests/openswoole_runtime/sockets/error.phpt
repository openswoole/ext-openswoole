--TEST--
openswoole_runtime/sockets: error
--SKIPIF--
<?php
require __DIR__ . '/../../include/skipif.inc';
skip_if_extension_not_exist('sockets');
skip_if_no_core_library();
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../../include/bootstrap.php';

use OpenSwoole\Runtime;



Runtime::enableCoroutine(Runtime::HOOK_ALL | Runtime::HOOK_SOCKETS);

$port = get_one_free_port();

co::run(function () use($port) {
    $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    socket_connect($sock, '127.0.0.1', $port);

    Assert::eq(socket_last_error(), SOCKET_ECONNREFUSED);
    Assert::eq(socket_last_error(), socket_last_error($sock));

    socket_clear_error();
    Assert::eq(socket_last_error(), 0);
    Assert::eq(socket_last_error($sock), SOCKET_ECONNREFUSED);
    socket_clear_error($sock);
    Assert::eq(socket_last_error($sock), 0);
});
echo "Done\n";
?>
--EXPECT--
Done
