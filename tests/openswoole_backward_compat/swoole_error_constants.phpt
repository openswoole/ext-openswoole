--TEST--
openswoole_backward_compat: SWOOLE_ERROR_* constant aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Sample of error constants
Assert::assert(defined('SWOOLE_ERROR_INVALID_PARAMS'));
Assert::assert(defined('SWOOLE_ERROR_MALLOC_FAIL'));
Assert::assert(defined('SWOOLE_ERROR_FILE_NOT_EXIST'));
Assert::assert(defined('SWOOLE_ERROR_FILE_EMPTY'));
Assert::assert(defined('SWOOLE_ERROR_DNSLOOKUP_RESOLVE_FAILED'));
Assert::assert(defined('SWOOLE_ERROR_SESSION_CLOSED'));
Assert::assert(defined('SWOOLE_ERROR_SESSION_NOT_EXIST'));
Assert::assert(defined('SWOOLE_ERROR_SOCKET_CLOSED'));
Assert::assert(defined('SWOOLE_ERROR_SERVER_INVALID_LISTEN_PORT'));
Assert::assert(defined('SWOOLE_ERROR_SERVER_TOO_MANY_LISTEN_PORT'));
Assert::assert(defined('SWOOLE_ERROR_SERVER_TOO_MANY_SOCKET'));
Assert::assert(defined('SWOOLE_ERROR_TASK_PACKAGE_TOO_BIG'));
Assert::assert(defined('SWOOLE_ERROR_CO_MUTEX_DOUBLE_UNLOCK'));

Assert::same(SWOOLE_ERROR_INVALID_PARAMS, OPENSWOOLE_ERROR_INVALID_PARAMS);
Assert::same(SWOOLE_ERROR_MALLOC_FAIL, OPENSWOOLE_ERROR_MALLOC_FAIL);
Assert::same(SWOOLE_ERROR_SESSION_CLOSED, OPENSWOOLE_ERROR_SESSION_CLOSED);
Assert::same(SWOOLE_ERROR_SOCKET_CLOSED, OPENSWOOLE_ERROR_SOCKET_CLOSED);
Assert::same(SWOOLE_ERROR_CO_MUTEX_DOUBLE_UNLOCK, OPENSWOOLE_ERROR_CO_MUTEX_DOUBLE_UNLOCK);

echo "PASSED\n";
?>
--EXPECT--
PASSED
