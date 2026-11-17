--TEST--
openswoole_table: an empty key does not end the iteration
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$table = new OpenSwoole\Table(1024);
$table->column('v', OpenSwoole\Table::TYPE_INT);
$table->create();
for ($i = 0; $i < 10; $i++) {
    $table->set("k{$i}", ['v' => $i]);
}
$table->set('', ['v' => 100]);
$sum = 0;
$n = 0;
foreach ($table as $key => $row) {
    $sum += $row['v'];
    $n++;
}
var_dump($n, $sum);
?>
--EXPECT--
int(11)
int(145)
