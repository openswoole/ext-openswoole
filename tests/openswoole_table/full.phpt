--TEST--
openswoole_table: set throws when the table is full and existing rows stay usable
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Table;

$table = new Table(64, 0.2);
$table->column('id', Table::TYPE_INT);
$table->column('name', Table::TYPE_STRING, 16);
$table->create();
Assert::same($table->getSize(), 64);

$exception = null;
$stored = [];
for ($i = 0; $i < 1000; $i++) {
    try {
        Assert::true($table->set("key{$i}", ['id' => $i, 'name' => "name{$i}"]));
        $stored[] = $i;
    } catch (OpenSwoole\Exception $e) {
        $exception = $e;
        break;
    }
}
Assert::notNull($exception);
Assert::same($exception->getMessage(), 'failed to set key value, try to increase the table_size');
Assert::same($exception->getCode(), OpenSwoole\Constant::ERROR_MALLOC_FAIL);

// 64 rows plus 20% of them for conflicts is the upper bound
Assert::lessThanEq(count($stored), 64 + 12);
Assert::greaterThan(count($stored), 1);
Assert::same($table->count(), count($stored));
Assert::false($table->exists("key{$i}"));

// every row stored before the failure is intact and can still be updated
foreach ($stored as $n) {
    Assert::same($table->get("key{$n}"), ['id' => $n, 'name' => "name{$n}"]);
    Assert::true($table->set("key{$n}", ['id' => $n * 2]));
    Assert::same($table->get("key{$n}", 'id'), $n * 2);
}
Assert::same($table->count(), count($stored));

// the key that did not fit keeps failing while the table is full
Assert::throws(function () use ($table, $i) {
    $table->set("key{$i}", ['id' => $i]);
}, OpenSwoole\Exception::class);
Assert::same($table->count(), count($stored));

// removing every row makes room again
foreach ($stored as $n) {
    Assert::true($table->del("key{$n}"));
}
Assert::same($table->count(), 0);
Assert::true($table->set("key{$i}", ['id' => $i]));
Assert::same($table->get("key{$i}", 'id'), $i);
Assert::same($table->count(), 1);
echo "DONE\n";
?>
--EXPECT--
DONE
