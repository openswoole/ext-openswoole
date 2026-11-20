--TEST--
openswoole_table: getSize rounds up to a power of two with a minimum of 64 rows
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Table;

foreach ([1, 64, 65, 100, 1000, 1024, 1025] as $size) {
    $table = new Table($size);
    $table->column('v', Table::TYPE_INT);
    $before = $table->getSize();
    $table->create();
    Assert::same($table->getSize(), $before);
    echo "$size => {$table->getSize()}\n";
    $table->destroy();
}
?>
--EXPECT--
1 => 64
64 => 64
65 => 128
100 => 128
1000 => 1024
1024 => 1024
1025 => 2048
