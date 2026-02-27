--TEST--
openswoole_timer: list
--CONFLICTS--
all
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
$s = microtime(true);
var_dump(OpenSwoole\Timer::stats());
for ($c = 1000; $c--;) {
    OpenSwoole\Timer::after(mt_rand(1, 1000), function () { });
}
var_dump(OpenSwoole\Timer::stats());
foreach (OpenSwoole\Timer::list() as $timer_id) {
    Assert::true(OpenSwoole\Timer::clear($timer_id));
}
OpenSwoole\Timer::after(100, function () {
    var_dump(OpenSwoole\Timer::stats());
});
OpenSwoole\Event::wait();
time_approximate(0.1, microtime(true) - $s);
?>
--EXPECTF--
array(3) {
  ["initialized"]=>
  bool(false)
  ["num"]=>
  int(0)
  ["round"]=>
  int(0)
}
array(3) {
  ["initialized"]=>
  bool(true)
  ["num"]=>
  int(1000)
  ["round"]=>
  int(0)
}
array(3) {
  ["initialized"]=>
  bool(true)
  ["num"]=>
  int(1)
  ["round"]=>
  int(1)
}
