--TEST--
openswoole_channel_coro: the bug about timeout timer
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc';
exit("skip for select");
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';
// this sript will hanging up and remove out of tests
// $chan = new chan(1);

// go(function () use ($chan) {
//     co::usleep(100000);
//     $chan->push('foo');
// });

// go(function () use ($chan) {
//     $read = [
//         $chan
//     ];
//     $write = [];
//     $ret = chan::select($read, $write, 0.1);
//     Assert::true($ret);
//     Assert::same(count($read), 1);
// });
?>
--EXPECT--
