--TEST--
openswoole_table: table stays consistent for concurrent writers after a child process exits
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

const WRITERS = 4;
const KEYS = 3000;

// many keys share a slot and need the pool allocation lock
$table = new OpenSwoole\Table(16384, 1);
$table->column('n', OpenSwoole\Table::TYPE_INT);
$table->create();

// a child that exits frees its copy of the table object
$pid = pcntl_fork();
if ($pid === 0) {
    exit(0);
}
pcntl_waitpid($pid, $status);

$pids = [];
for ($w = 0; $w < WRITERS; $w++) {
    $pid = pcntl_fork();
    if ($pid === 0) {
        for ($i = 0; $i < KEYS; $i++) {
            $table->set("w{$w}_{$i}", ['n' => $i]);
        }
        exit(0);
    }
    $pids[] = $pid;
}
foreach ($pids as $pid) {
    pcntl_waitpid($pid, $status);
}

Assert::same($table->count(), WRITERS * KEYS);
$missing = 0;
for ($w = 0; $w < WRITERS; $w++) {
    for ($i = 0; $i < KEYS; $i++) {
        if ($table->get("w{$w}_{$i}", 'n') !== $i) {
            $missing++;
        }
    }
}
Assert::same($missing, 0);
echo "DONE\n";
?>
--EXPECT--
DONE
