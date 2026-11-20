--TEST--
openswoole_table: keys up to 63 bytes are kept and longer keys warn and are cut
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$table = new OpenSwoole\Table(1024);
$table->column('v', OpenSwoole\Table::TYPE_INT);
$table->create();

$max = str_repeat('k', 62) . 'x';
Assert::same(strlen($max), 63);
Assert::true($table->set($max, ['v' => 63]));
Assert::same($table->get($max, 'v'), 63);
// a different 62-byte prefix of the same bytes is a different key
Assert::false($table->exists(substr($max, 0, 62)));

$long = $max . 'yz';
Assert::true($table->set($long, ['v' => 65]));
// the 65-byte key is stored as its 63-byte prefix, which is $max
Assert::same($table->count(), 1);
Assert::same($table->get($max, 'v'), 65);
foreach ($table as $key => $row) {
    Assert::same($key, $max);
}
?>
--EXPECTF--
Warning: OpenSwoole\Table::set(): key[%s] is too long in %s on line %d
