--TEST--
openswoole_table: incr() and decr() on a bad column do not create the row
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$table = new OpenSwoole\Table(1024);
$table->column('n', OpenSwoole\Table::TYPE_INT);
$table->column('s', OpenSwoole\Table::TYPE_STRING, 8);
$table->create();
var_dump(@$table->incr('a', 'missing'));
var_dump(@$table->decr('b', 's'));
var_dump($table->count(), $table->exists('a'), $table->exists('b'));
var_dump($table->incr('c', 'n', 5), $table->decr('c', 'n'));
?>
--EXPECT--
bool(false)
bool(false)
int(0)
bool(false)
bool(false)
int(5)
int(4)
