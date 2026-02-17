--TEST--
swoole_coroutine_fiber: coroutine stats with fiber context
--SKIPIF--
<?php require __DIR__ . '/../include/skipif.inc'; ?>
<?php
ob_start();
phpinfo(INFO_MODULES);
$info = ob_get_clean();
if (strpos($info, 'enabled with fiber context') === false) {
    die('skip fiber context not enabled');
}
?>
--FILE--
<?php declare(strict_types = 1);
require __DIR__ . '/../include/bootstrap.php';

Assert::same(Co::stats()['coroutine_num'], 0);
Assert::same(Co::stats()['coroutine_peak_num'], 0);
go(function () {
    Assert::same(Co::stats()['coroutine_num'], 1);
    Assert::same(Co::stats()['coroutine_peak_num'], 1);
    co::usleep(500000);
    Assert::same(Co::stats()['coroutine_num'], 2);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
});
go(function () {
    Assert::same(Co::stats()['coroutine_num'], 2);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
    co::usleep(500000);
    Assert::same(Co::stats()['coroutine_num'], 1);
    Assert::same(Co::stats()['coroutine_peak_num'], 2);
});
Assert::same(Co::stats()['coroutine_num'], 2);
Assert::same(Co::stats()['coroutine_peak_num'], 2);
echo "DONE\n";
?>
--EXPECT--
DONE
