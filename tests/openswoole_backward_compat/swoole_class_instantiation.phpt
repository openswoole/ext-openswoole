--TEST--
openswoole_backward_compat: instantiate classes via Swoole\* aliases
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

// Atomic via Swoole namespace
$atomic = new Swoole\Atomic(0);
$atomic->add(42);
Assert::same($atomic->get(), 42);

// Atomic\Long via Swoole namespace
$long = new Swoole\Atomic\Long(0);
$long->add(100);
Assert::same($long->get(), 100);

// Table via Swoole namespace
$table = new Swoole\Table(64);
$table->column('name', Swoole\Table::TYPE_STRING, 64);
$table->column('score', Swoole\Table::TYPE_INT);
$table->create();
$table->set('user1', ['name' => 'test', 'score' => 99]);
Assert::same($table->get('user1', 'name'), 'test');
Assert::same($table->get('user1', 'score'), 99);

// Lock via Swoole namespace
$lock = new Swoole\Lock(SWOOLE_MUTEX);
Assert::assert($lock instanceof OpenSwoole\Lock);

// Channel via Swoole namespace
$chan = new Swoole\Coroutine\Channel(1);
Assert::assert($chan instanceof OpenSwoole\Coroutine\Channel);

// WebSocket Frame via Swoole namespace
$frame = new Swoole\WebSocket\Frame();
Assert::assert($frame instanceof OpenSwoole\WebSocket\Frame);

echo "PASSED\n";
?>
--EXPECT--
PASSED
