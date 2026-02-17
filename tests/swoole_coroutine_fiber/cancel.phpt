--TEST--
swoole_coroutine_fiber: cancel coroutine with fiber context
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

co::run(function () {
    $cid = go(function () {
        $ret = Co::yield();
        Assert::false($ret);
        Assert::true(Co::isCanceled());
        echo "canceled\n";
    });

    go(function () use ($cid) {
        Assert::true(Co::cancel($cid));
        echo "cancel sent\n";
    });
});
?>
--EXPECT--
canceled
cancel sent
