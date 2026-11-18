--TEST--
openswoole_table: concurrent incr() from several processes loses no update
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const N = 20000;
const P = 4;

$table = new OpenSwoole\Table(1024);
$table->column('n', OpenSwoole\Table::TYPE_INT);
$table->create();
for ($p = 0; $p < P; $p++) {
    (new OpenSwoole\Process(function () use ($table) {
        for ($i = 0; $i < N; $i++) {
            $table->incr('counter', 'n');
        }
    }, false, 0))->start();
}
for ($p = 0; $p < P; $p++) {
    OpenSwoole\Process::wait();
}
var_dump($table->get('counter', 'n'));
?>
--EXPECT--
int(80000)
