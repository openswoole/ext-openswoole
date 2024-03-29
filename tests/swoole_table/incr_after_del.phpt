--TEST--
swoole_table: incr after del
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$table = new \OpenSwoole\Table(1024);
$table->column('id', \OpenSwoole\Table::TYPE_INT);
$table->create();

$table->set('1', ['id' => 1,]);

for($i=0;$i<10;$i++) {
  $table->incr('1', 'id', 1);
}

Assert::eq($table->get('1')['id'], 11);
Assert::true($table->del('1'));
Assert::false($table->get('1'));

for($i = 0; $i < 10; $i++) {
  $table->incr('1', 'id', 1);
}
Assert::eq($table->get('1')['id'], 10);
?>
--EXPECT--
