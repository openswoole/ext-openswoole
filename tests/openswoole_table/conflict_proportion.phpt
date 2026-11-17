--TEST--
openswoole_table: conflictProportion sets the memory reserved for hash conflicts
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Table;

function table_memory(float $proportion): int
{
    $table = new Table(1024, $proportion);
    $table->column('id', Table::TYPE_INT, 8);
    $table->column('name', Table::TYPE_STRING, 64);
    $table->create();
    // fewer conflict rows leave less room for collisions, so the table is not filled up
    for ($i = 0; $i < 100; $i++) {
        Assert::true($table->set("key{$i}", ['id' => $i, 'name' => "name{$i}"]));
    }
    Assert::same($table->get('key99')['id'], 99);
    return $table->getMemorySize();
}

$default = table_memory(1.0);
$small = table_memory(0.2);
$tooSmall = table_memory(0.01);
var_dump($small < $default);
var_dump($tooSmall === $small);
var_dump(table_memory(2.0) === $default);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
