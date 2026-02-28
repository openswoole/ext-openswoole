--TEST--
openswoole_backward_compat: SWOOLE_* constant aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Core constants
Assert::assert(defined('SWOOLE_BASE'));
Assert::assert(defined('SWOOLE_PROCESS'));
Assert::assert(defined('SWOOLE_SYNC'));
Assert::assert(defined('SWOOLE_ASYNC'));
Assert::assert(defined('SWOOLE_KEEP'));
Assert::same(SWOOLE_BASE, OPENSWOOLE_BASE);
Assert::same(SWOOLE_PROCESS, OPENSWOOLE_PROCESS);
Assert::same(SWOOLE_SYNC, OPENSWOOLE_SYNC);
Assert::same(SWOOLE_ASYNC, OPENSWOOLE_ASYNC);
Assert::same(SWOOLE_KEEP, OPENSWOOLE_KEEP);

// Socket types
Assert::assert(defined('SWOOLE_SOCK_TCP'));
Assert::assert(defined('SWOOLE_SOCK_TCP6'));
Assert::assert(defined('SWOOLE_SOCK_UDP'));
Assert::assert(defined('SWOOLE_SOCK_UDP6'));
Assert::assert(defined('SWOOLE_SOCK_UNIX_STREAM'));
Assert::assert(defined('SWOOLE_SOCK_UNIX_DGRAM'));
Assert::same(SWOOLE_SOCK_TCP, OPENSWOOLE_SOCK_TCP);
Assert::same(SWOOLE_SOCK_TCP6, OPENSWOOLE_SOCK_TCP6);
Assert::same(SWOOLE_SOCK_UDP, OPENSWOOLE_SOCK_UDP);
Assert::same(SWOOLE_SOCK_UDP6, OPENSWOOLE_SOCK_UDP6);
Assert::same(SWOOLE_SOCK_UNIX_STREAM, OPENSWOOLE_SOCK_UNIX_STREAM);
Assert::same(SWOOLE_SOCK_UNIX_DGRAM, OPENSWOOLE_SOCK_UNIX_DGRAM);

// IPC types
Assert::assert(defined('SWOOLE_IPC_NONE'));
Assert::assert(defined('SWOOLE_IPC_MSGQUEUE'));
Assert::assert(defined('SWOOLE_IPC_SOCKET'));
Assert::same(SWOOLE_IPC_NONE, OPENSWOOLE_IPC_NONE);
Assert::same(SWOOLE_IPC_MSGQUEUE, OPENSWOOLE_IPC_MSGQUEUE);
Assert::same(SWOOLE_IPC_SOCKET, OPENSWOOLE_IPC_SOCKET);

// Version
Assert::assert(defined('SWOOLE_VERSION'));
Assert::assert(defined('SWOOLE_VERSION_ID'));
Assert::assert(defined('SWOOLE_MAJOR_VERSION'));
Assert::assert(defined('SWOOLE_MINOR_VERSION'));
Assert::assert(defined('SWOOLE_RELEASE_VERSION'));
Assert::same(SWOOLE_VERSION, OPENSWOOLE_VERSION);
Assert::same(SWOOLE_VERSION_ID, OPENSWOOLE_VERSION_ID);
Assert::same(SWOOLE_MAJOR_VERSION, OPENSWOOLE_MAJOR_VERSION);
Assert::same(SWOOLE_MINOR_VERSION, OPENSWOOLE_MINOR_VERSION);
Assert::same(SWOOLE_RELEASE_VERSION, OPENSWOOLE_RELEASE_VERSION);

// Log levels
Assert::assert(defined('SWOOLE_LOG_DEBUG'));
Assert::assert(defined('SWOOLE_LOG_INFO'));
Assert::assert(defined('SWOOLE_LOG_NOTICE'));
Assert::assert(defined('SWOOLE_LOG_WARNING'));
Assert::assert(defined('SWOOLE_LOG_ERROR'));
Assert::assert(defined('SWOOLE_LOG_NONE'));
Assert::same(SWOOLE_LOG_DEBUG, OPENSWOOLE_LOG_DEBUG);
Assert::same(SWOOLE_LOG_INFO, OPENSWOOLE_LOG_INFO);
Assert::same(SWOOLE_LOG_WARNING, OPENSWOOLE_LOG_WARNING);
Assert::same(SWOOLE_LOG_ERROR, OPENSWOOLE_LOG_ERROR);

// Event flags
Assert::assert(defined('SWOOLE_EVENT_READ'));
Assert::assert(defined('SWOOLE_EVENT_WRITE'));
Assert::same(SWOOLE_EVENT_READ, OPENSWOOLE_EVENT_READ);
Assert::same(SWOOLE_EVENT_WRITE, OPENSWOOLE_EVENT_WRITE);

// Lock types
Assert::assert(defined('SWOOLE_MUTEX'));
Assert::assert(defined('SWOOLE_RWLOCK'));
Assert::assert(defined('SWOOLE_SPINLOCK'));
Assert::assert(defined('SWOOLE_FILELOCK'));
Assert::assert(defined('SWOOLE_SEM'));
Assert::same(SWOOLE_MUTEX, OPENSWOOLE_MUTEX);
Assert::same(SWOOLE_RWLOCK, OPENSWOOLE_RWLOCK);
Assert::same(SWOOLE_SPINLOCK, OPENSWOOLE_SPINLOCK);
Assert::same(SWOOLE_FILELOCK, OPENSWOOLE_FILELOCK);
Assert::same(SWOOLE_SEM, OPENSWOOLE_SEM);

// Task constants
Assert::assert(defined('SWOOLE_TASK_NOREPLY'));
Assert::assert(defined('SWOOLE_TASK_NONBLOCK'));
Assert::assert(defined('SWOOLE_TASK_CALLBACK'));
Assert::assert(defined('SWOOLE_TASK_COROUTINE'));
Assert::same(SWOOLE_TASK_NOREPLY, OPENSWOOLE_TASK_NOREPLY);
Assert::same(SWOOLE_TASK_NONBLOCK, OPENSWOOLE_TASK_NONBLOCK);

// Worker states
Assert::assert(defined('SWOOLE_WORKER_BUSY'));
Assert::assert(defined('SWOOLE_WORKER_IDLE'));
Assert::assert(defined('SWOOLE_WORKER_EXIT'));
Assert::same(SWOOLE_WORKER_BUSY, OPENSWOOLE_WORKER_BUSY);
Assert::same(SWOOLE_WORKER_IDLE, OPENSWOOLE_WORKER_IDLE);
Assert::same(SWOOLE_WORKER_EXIT, OPENSWOOLE_WORKER_EXIT);

// Coroutine states
Assert::assert(defined('SWOOLE_CORO_INIT'));
Assert::assert(defined('SWOOLE_CORO_RUNNING'));
Assert::assert(defined('SWOOLE_CORO_WAITING'));
Assert::assert(defined('SWOOLE_CORO_END'));
Assert::same(SWOOLE_CORO_INIT, OPENSWOOLE_CORO_INIT);
Assert::same(SWOOLE_CORO_RUNNING, OPENSWOOLE_CORO_RUNNING);
Assert::same(SWOOLE_CORO_WAITING, OPENSWOOLE_CORO_WAITING);
Assert::same(SWOOLE_CORO_END, OPENSWOOLE_CORO_END);

// Channel states
Assert::assert(defined('SWOOLE_CHANNEL_OK'));
Assert::assert(defined('SWOOLE_CHANNEL_CLOSED'));
Assert::assert(defined('SWOOLE_CHANNEL_TIMEOUT'));
Assert::assert(defined('SWOOLE_CHANNEL_CANCELED'));
Assert::same(SWOOLE_CHANNEL_OK, OPENSWOOLE_CHANNEL_OK);
Assert::same(SWOOLE_CHANNEL_CLOSED, OPENSWOOLE_CHANNEL_CLOSED);
Assert::same(SWOOLE_CHANNEL_TIMEOUT, OPENSWOOLE_CHANNEL_TIMEOUT);
Assert::same(SWOOLE_CHANNEL_CANCELED, OPENSWOOLE_CHANNEL_CANCELED);

// Timer limits
Assert::assert(defined('SWOOLE_TIMER_MIN_MS'));
Assert::assert(defined('SWOOLE_TIMER_MIN_SEC'));
Assert::assert(defined('SWOOLE_TIMER_MAX_MS'));
Assert::assert(defined('SWOOLE_TIMER_MAX_SEC'));
Assert::same(SWOOLE_TIMER_MIN_MS, OPENSWOOLE_TIMER_MIN_MS);

echo "PASSED\n";
?>
--EXPECT--
PASSED
