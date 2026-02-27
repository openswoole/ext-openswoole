--TEST--
openswoole_backward_compat: swoole_* snake_case class aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// swoole_* snake_case class aliases
Assert::assert(class_exists('swoole_server'));
Assert::assert(class_exists('swoole_client'));
Assert::assert(class_exists('swoole_event'));
Assert::assert(class_exists('swoole_timer'));
Assert::assert(class_exists('swoole_table'));
Assert::assert(class_exists('swoole_lock'));
Assert::assert(class_exists('swoole_atomic'));
Assert::assert(class_exists('swoole_atomic_long'));
Assert::assert(class_exists('swoole_process'));
Assert::assert(class_exists('swoole_process_pool'));
Assert::assert(class_exists('swoole_runtime'));
Assert::assert(class_exists('swoole_channel'));
Assert::assert(class_exists('swoole_error'));
Assert::assert(class_exists('swoole_channel_coro'));
Assert::assert(class_exists('swoole_http_server'));
Assert::assert(class_exists('swoole_http_request'));
Assert::assert(class_exists('swoole_http_response'));
Assert::assert(class_exists('swoole_websocket_server'));
Assert::assert(class_exists('swoole_websocket_frame'));
Assert::assert(class_exists('swoole_websocket_closeframe'));
Assert::assert(class_exists('swoole_server_port'));
Assert::assert(class_exists('swoole_server_task'));
Assert::assert(class_exists('swoole_exit_exception'));

// openswoole_* snake_case class aliases
Assert::assert(class_exists('openswoole_server'));
Assert::assert(class_exists('openswoole_client'));
Assert::assert(class_exists('openswoole_event'));
Assert::assert(class_exists('openswoole_timer'));
Assert::assert(class_exists('openswoole_table'));
Assert::assert(class_exists('openswoole_lock'));
Assert::assert(class_exists('openswoole_atomic'));
Assert::assert(class_exists('openswoole_atomic_long'));
Assert::assert(class_exists('openswoole_process'));
Assert::assert(class_exists('openswoole_process_pool'));
Assert::assert(class_exists('openswoole_runtime'));
Assert::assert(class_exists('openswoole_channel'));

// Verify they all resolve to the same class
Assert::same(swoole_server::class, openswoole_server::class);
Assert::same(swoole_table::class, openswoole_table::class);
Assert::same(swoole_timer::class, openswoole_timer::class);
Assert::same(swoole_http_server::class, openswoole_http_server::class);

echo "PASSED\n";
?>
--EXPECT--
PASSED
