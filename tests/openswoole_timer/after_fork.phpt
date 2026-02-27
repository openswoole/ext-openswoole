--TEST--
openswoole_timer: after fork
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$id = OpenSwoole\Timer::after(1, function () { echo 'never here' . PHP_EOL; });
if (Assert::greaterThan($id, 0)) {
    $process = new OpenSwoole\Process(function () use ($id) {
        // timer will be removed before fork
        Assert::false(OpenSwoole\Timer::exists($id));
        echo "DONE\n";
    });
    $process->start();
    $process::wait();
    Assert::true(OpenSwoole\Timer::clear($id));
}
OpenSwoole\Event::wait();
?>
--EXPECT--
DONE
