--TEST--
openswoole_table: keys longer than the key size are found by the key that is returned
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$table = new OpenSwoole\Table(1024);
$table->column('v', OpenSwoole\Table::TYPE_INT);
$table->create();

$long = str_repeat('a', 70);
Assert::true(@$table->set($long, ['v' => 1]));
var_dump($table->get($long, 'v'));
foreach ($table as $key => $row) {
    var_dump(strlen($key));
    var_dump($table->get($key, 'v'));
    var_dump($table->exists($key));
}
// the same 63-byte prefix is the same key
Assert::true(@$table->set(str_repeat('a', 80), ['v' => 2]));
var_dump($table->count(), $table->get($long, 'v'));
var_dump($table->del($long), $table->count());
?>
--EXPECT--
int(1)
int(63)
int(1)
bool(true)
int(1)
int(2)
bool(true)
int(0)
