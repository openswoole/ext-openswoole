--TEST--
openswoole_timer: clear timer not exist
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
OpenSwoole\Timer::after(10, function () {
    Assert::assert(0); // never here
});
for ($n = MAX_REQUESTS; $n--;) {
    Assert::assert(OpenSwoole\Timer::clear($n) === ($n === 1 ? true : false));
}
OpenSwoole\Event::wait();
echo "DONE\n";
?>
--EXPECT--
DONE
