--TEST--
openswoole_backward_compat: swoole_* function aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Coroutine functions
Assert::assert(function_exists('swoole_coroutine_create'));
Assert::assert(function_exists('swoole_coroutine_defer'));
Assert::assert(function_exists('openswoole_coroutine_create'));
Assert::assert(function_exists('openswoole_coroutine_defer'));

// Shorthand aliases
Assert::assert(function_exists('go'));
Assert::assert(function_exists('defer'));

// Event functions
Assert::assert(function_exists('swoole_event_add'));
Assert::assert(function_exists('swoole_event_del'));
Assert::assert(function_exists('swoole_event_set'));
Assert::assert(function_exists('swoole_event_isset'));
Assert::assert(function_exists('swoole_event_write'));
Assert::assert(function_exists('swoole_event_wait'));
Assert::assert(function_exists('swoole_event_defer'));
Assert::assert(function_exists('swoole_event_cycle'));
Assert::assert(function_exists('swoole_event_dispatch'));
Assert::assert(function_exists('swoole_event_exit'));

// Timer functions
Assert::assert(function_exists('swoole_timer_tick'));
Assert::assert(function_exists('swoole_timer_after'));
Assert::assert(function_exists('swoole_timer_exists'));
Assert::assert(function_exists('swoole_timer_clear'));
Assert::assert(function_exists('swoole_timer_clear_all'));
Assert::assert(function_exists('swoole_timer_info'));
Assert::assert(function_exists('swoole_timer_list'));
Assert::assert(function_exists('swoole_timer_stats'));

// Verify swoole_* and openswoole_* event functions both exist
Assert::assert(function_exists('openswoole_event_add'));
Assert::assert(function_exists('openswoole_event_del'));
Assert::assert(function_exists('openswoole_event_wait'));
Assert::assert(function_exists('openswoole_timer_tick'));
Assert::assert(function_exists('openswoole_timer_after'));
Assert::assert(function_exists('openswoole_timer_clear'));

echo "PASSED\n";
?>
--EXPECT--
PASSED
