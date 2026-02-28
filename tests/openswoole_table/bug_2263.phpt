--TEST--
openswoole_table: bug_2263
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

use OpenSwoole\Table;

$table = new Table(1024);
$table->column('data', Table::TYPE_STRING, 1);
$table->create();

$table->set("1234567890", ['data' => '1']);
$table->set("44984", ['data' => '2']);

$table->del("1234567890");

foreach($table as $ip => $row) {
	echo $ip."\n";
}

?>
--EXPECT--
44984
