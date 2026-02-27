--TEST--
openswoole_coroutine_util: list
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

$co_list = [];

foreach(range(1, 10) as $i) {
    $co_list[] = go(function () use ($i) {
        Co::usleep(400000);
    });
}

go(function () use ($co_list) {
    Co::usleep(200000);
    $coros = Co::list();
    $list_2 = iterator_to_array($coros);
    Assert::same(array_values(array_diff($list_2, $co_list)), [Co::getCid(),]);
});

openswoole_event_wait();

?>
--EXPECT--
