--TEST--
swoole_table: big size
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$table = new \OpenSwoole\Table(1);
$table->column('string', \OpenSwoole\Table::TYPE_STRING, $string_size = mt_rand(2, 6) * 0xffff);
$table->column('int', \OpenSwoole\Table::TYPE_INT, 8);
$table->create();
$data = [
    'string' => str_repeat('S', $string_size),
    'int' => PHP_INT_MAX
];
$table->set('test', $data);
Assert::same($table->get('test'), $data);
var_dump($table->get('test'));
?>
--EXPECTF--
array(2) {
  ["string"]=>
  string(%d) "%s"
  ["int"]=>
  int(%d)
}
