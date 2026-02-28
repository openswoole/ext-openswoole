--TEST--
openswoole_backward_compat: SWOOLE_HOOK_* constant aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::assert(defined('SWOOLE_HOOK_ALL'));
Assert::assert(defined('SWOOLE_HOOK_TCP'));
Assert::assert(defined('SWOOLE_HOOK_UDP'));
Assert::assert(defined('SWOOLE_HOOK_UNIX'));
Assert::assert(defined('SWOOLE_HOOK_UDG'));
Assert::assert(defined('SWOOLE_HOOK_SSL'));
Assert::assert(defined('SWOOLE_HOOK_TLS'));
Assert::assert(defined('SWOOLE_HOOK_SLEEP'));
Assert::assert(defined('SWOOLE_HOOK_FILE'));
Assert::assert(defined('SWOOLE_HOOK_STDIO'));
Assert::assert(defined('SWOOLE_HOOK_PROC'));
Assert::assert(defined('SWOOLE_HOOK_SOCKETS'));
Assert::assert(defined('SWOOLE_HOOK_STREAM_FUNCTION'));
Assert::assert(defined('SWOOLE_HOOK_STREAM_SELECT'));
Assert::assert(defined('SWOOLE_HOOK_BLOCKING_FUNCTION'));
Assert::assert(defined('SWOOLE_HOOK_CURL'));
Assert::assert(defined('SWOOLE_HOOK_NATIVE_CURL'));

Assert::same(SWOOLE_HOOK_ALL, OPENSWOOLE_HOOK_ALL);
Assert::same(SWOOLE_HOOK_TCP, OPENSWOOLE_HOOK_TCP);
Assert::same(SWOOLE_HOOK_UDP, OPENSWOOLE_HOOK_UDP);
Assert::same(SWOOLE_HOOK_UNIX, OPENSWOOLE_HOOK_UNIX);
Assert::same(SWOOLE_HOOK_UDG, OPENSWOOLE_HOOK_UDG);
Assert::same(SWOOLE_HOOK_SSL, OPENSWOOLE_HOOK_SSL);
Assert::same(SWOOLE_HOOK_TLS, OPENSWOOLE_HOOK_TLS);
Assert::same(SWOOLE_HOOK_SLEEP, OPENSWOOLE_HOOK_SLEEP);
Assert::same(SWOOLE_HOOK_FILE, OPENSWOOLE_HOOK_FILE);
Assert::same(SWOOLE_HOOK_STDIO, OPENSWOOLE_HOOK_STDIO);
Assert::same(SWOOLE_HOOK_PROC, OPENSWOOLE_HOOK_PROC);
Assert::same(SWOOLE_HOOK_SOCKETS, OPENSWOOLE_HOOK_SOCKETS);
Assert::same(SWOOLE_HOOK_STREAM_FUNCTION, OPENSWOOLE_HOOK_STREAM_FUNCTION);
Assert::same(SWOOLE_HOOK_STREAM_SELECT, OPENSWOOLE_HOOK_STREAM_SELECT);
Assert::same(SWOOLE_HOOK_BLOCKING_FUNCTION, OPENSWOOLE_HOOK_BLOCKING_FUNCTION);
Assert::same(SWOOLE_HOOK_CURL, OPENSWOOLE_HOOK_CURL);
Assert::same(SWOOLE_HOOK_NATIVE_CURL, OPENSWOOLE_HOOK_NATIVE_CURL);

echo "PASSED\n";
?>
--EXPECT--
PASSED
