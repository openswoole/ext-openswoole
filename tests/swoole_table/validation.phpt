--TEST--
swoole_table: validation
--SKIPIF--
<?php require  __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$table = new Swoole\Table(10);
$table->column('a', Swoole\Table::TYPE_INT);
$table->column('b', Swoole\Table::TYPE_FLOAT);
$table->column('c', Swoole\Table::TYPE_STRING, 8);
$table->create();
try{
	$table->set('a', array('a' => '1', 'b' => 1.1, 'c' => '1'));
} catch(\Swoole\Exception $e) {
	echo $e->getMessage()."\n";
}
try{
	$table->set('a', array('a' => '1', 'b' => 1, 'c' => '1'));
} catch(\Swoole\Exception $e) {
	echo $e->getMessage()."\n";
}
try{
	$table->set('a', array('a' => '1', 'b' => 1, 'c' => 1));
} catch(\Swoole\Exception $e) {
	echo $e->getMessage()."\n";
}
try{
	$table->set('a', array('a' => 1, 'b' => 1.1, 'c' => '111111111'));
} catch(\Swoole\Exception $e) {
	echo $e->getMessage()."\n";
}

echo "DONE";
?>
--EXPECTF--
[key=a,field=a] type error for TYPE_INT column
[key=a,field=b] type error for TYPE_FLOAT column
[key=a,field=c] type error for TYPE_STRING column
[%s]	WARNING	TableRow::set_value(): [key=a,field=c]string value is too long
[key=a,field=c] value is too long: 9, maximum length: 8
DONE
