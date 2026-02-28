--TEST--
openswoole_backward_compat: Swoole\* class aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Core classes - Swoole\* namespace aliases
Assert::assert(class_exists('Swoole\Server'));
Assert::assert(class_exists('Swoole\Client'));
Assert::assert(class_exists('Swoole\Event'));
Assert::assert(class_exists('Swoole\Timer'));
Assert::assert(class_exists('Swoole\Table'));
Assert::assert(class_exists('Swoole\Lock'));
Assert::assert(class_exists('Swoole\Atomic'));
Assert::assert(class_exists('Swoole\Atomic\Long'));
Assert::assert(class_exists('Swoole\Process'));
Assert::assert(class_exists('Swoole\Process\Pool'));
Assert::assert(class_exists('Swoole\Runtime'));
Assert::assert(class_exists('Swoole\Error'));
Assert::assert(class_exists('Swoole\Exception'));

// Coroutine classes
Assert::assert(class_exists('Swoole\Coroutine'));
Assert::assert(class_exists('Swoole\Coroutine\Channel'));
Assert::assert(class_exists('Swoole\Coroutine\Client'));
Assert::assert(class_exists('Swoole\Coroutine\Socket'));
Assert::assert(class_exists('Swoole\Coroutine\Context'));
Assert::assert(class_exists('Swoole\Coroutine\Iterator'));
Assert::assert(class_exists('Swoole\Coroutine\System'));
Assert::assert(class_exists('Swoole\Coroutine\Scheduler'));

// HTTP classes
Assert::assert(class_exists('Swoole\Http\Server'));
Assert::assert(class_exists('Swoole\Http\Request'));
Assert::assert(class_exists('Swoole\Http\Response'));
Assert::assert(class_exists('Swoole\Coroutine\Http\Client'));

// WebSocket classes
Assert::assert(class_exists('Swoole\WebSocket\Server'));
Assert::assert(class_exists('Swoole\WebSocket\Frame'));
Assert::assert(class_exists('Swoole\WebSocket\CloseFrame'));

// Server sub-classes
Assert::assert(class_exists('Swoole\Server\Port'));
Assert::assert(class_exists('Swoole\Server\Task'));
Assert::assert(class_exists('Swoole\Server\Event'));
Assert::assert(class_exists('Swoole\Server\Packet'));
Assert::assert(class_exists('Swoole\Server\PipeMessage'));
Assert::assert(class_exists('Swoole\Server\StatusInfo'));
Assert::assert(class_exists('Swoole\Server\TaskResult'));

// Connection iterator
Assert::assert(class_exists('Swoole\Connection\Iterator'));

// Verify they resolve to the same class as OpenSwoole\* (use ReflectionClass since ::class is compile-time)
Assert::same((new ReflectionClass('Swoole\Server'))->getName(), 'OpenSwoole\Server');
Assert::same((new ReflectionClass('Swoole\Coroutine'))->getName(), 'OpenSwoole\Coroutine');
Assert::same((new ReflectionClass('Swoole\Table'))->getName(), 'OpenSwoole\Table');
Assert::same((new ReflectionClass('Swoole\Timer'))->getName(), 'OpenSwoole\Timer');
Assert::same((new ReflectionClass('Swoole\Http\Server'))->getName(), 'OpenSwoole\Http\Server');
Assert::same((new ReflectionClass('Swoole\WebSocket\Server'))->getName(), 'OpenSwoole\WebSocket\Server');

// Co short alias
Assert::assert(class_exists('Co'));
Assert::same((new ReflectionClass('Co'))->getName(), 'OpenSwoole\Coroutine');

echo "PASSED\n";
?>
--EXPECT--
PASSED
