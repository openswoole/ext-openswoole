--TEST--
swoole_coroutine_fiber: yield and resume
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

$result = [];

$cid = go(function () use (&$result) {
    $result[] = 'coro_before_yield';
    Co::yield();
    $result[] = 'coro_after_resume';
});

$result[] = 'main_before_resume';
Co::resume($cid);
$result[] = 'main_after_resume';

Assert::same($result, [
    'coro_before_yield',
    'main_before_resume',
    'coro_after_resume',
    'main_after_resume',
]);
echo "DONE\n";
?>
--EXPECT--
DONE
